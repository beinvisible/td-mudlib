/* Abenteurer-Spellbook neu, komplett neu geschrieben da mir der
 * Spaghetticode einfach zu uebel wurde.
 *
 * Urspruengliche Version von Boing@MorgenGrauen
 * Weitere Autoren:
 *   - Mupfel@AnderLand
 *   - Woody@SilberLand
 *   - Feyaria@SilberLand
 *
*/
#include <moving.h>
#include <properties.h>
#include <defines.h>
#include <vt100.h>
#include <combat.h>
#include <class.h>
#include <new_skills.h>
//#include "/gilden/files.abenteurer/abenteurer.h"

inherit "/std/spellbook";
inherit "/spellbooks/std/spellbook_helper";

void create()
{
    string	lines;

    ::create();

    /* Globales Zeugs ... */
    SetProp(P_GLOBAL_SKILLPROPS,
	([
	    SI_SPELLFATIGUE:			2,
	    SI_SKILLLEARN:			3,
	    OFFSET(SI_SKILLLEARN):		8,
            SI_USR:                             0, // Spellcounter init
	    SI_SKILLLEARN_ATTRIBUTE:		A_INT,
	    SI_PRIM_ATTR:			A_INT,
	    SI_SECU_ATTR:			A_INT,

	    SI_TIME_MSG:
		"Du musst Dich noch besser konzentrieren.\n",
	    SI_SP_LOW_MSG:
		"Du fühlst, dass Deine magischen Energien zur Zeit nicht "+
		"ausreichen.\n",
	    SI_LEARN_MSG:
		"Du erhältst durch das Misslingen neue Einsichten, "+
		"wie der Zauber funktioniert.\n",

	    SI_FAILLEARN_MSG:
		"Dir ist nicht ganz klar, wie der Fehler passiert ist.\n"
	]));

    /* Nun die Sprueche */
    AddSpell("blitz", 3,
	([
	    SI_SKILLRESTR_LEARN:		([P_LEVEL: 2]),
	    SI_PRIM_ATTR:	A_INT,
	    SI_SECU_ATTR:	A_INT,
	    SI_PREPARE_TIME:	2,
	    SI_PREPARE_MSG:
		"Du versuchst, die Details Deiner Umgebung zu erhaschen.\n",
	    SI_FAIL_MSG:
		"Deine Konzentration reicht nicht, um Dich im Dunkeln "+
		"umzublicken.\n",
	    SI_LEARN_MSG:
		"Du schaffst es aber, die Helligkeit des Spruches zu "+
		"erhöhen.\n"
	]));

    AddSpell("identifiziere", 12,
	([
	    SI_SKILLRESTR_LEARN:		([P_LEVEL: 4]),
	    SI_PRIM_ATTR:	A_INT,
	    SI_SECU_ATTR:	A_INT,
	]));

    AddSpell("schätz", 5,
	([
	    SI_SKILLRESTR_LEARN:		([P_LEVEL: 6]),
	    SI_PRIM_ATTR:	A_INT,
	    SI_SECU_ATTR:	A_DEX,
	    SI_PREPARE_TIME:	1,
	    SI_PREPARE_MSG:
		"Du musterst Dein Ziel prüfend von oben bis unten.\n",
	    SI_FAIL_MSG:
		"Ein tieferer Einblick bleibt Dir verwehrt.\n"
	]));

    AddSpell("pfeil", 10,
	([
	    SI_SKILLRESTR_LEARN:		([P_LEVEL: 11]),
	    SI_PRIM_ATTR:	A_INT,
	    SI_SECU_ATTR:	A_DEX,
	    SI_SKILLRESTR_USE:
		([SR_FREE_HANDS: 1]),
	    SI_SKILLDAMAGE_TYPE:	({ DT_MAGIC, DT_PIERCE }),
	    SI_SKILLDAMAGE:	200,
	    SI_TIME_MSG:
		"Der nächste Pfeil war noch nicht fertig geformt.\n",
	    SI_FAIL_MSG:
		"Der magische Pfeil fliegt meilenweit daneben und "+
		"löst sich dann auf.\n",
	    SI_LEARN_MSG: break_string(
		"Beim letzten Werfen des Pfeiles hast Du erkannt, daß "+
		"Dein Pfeil noch nicht ganz perfekt war und merzt die "+
		"Schwachstelle aus.")
	]));

    AddSpell("feuerball", 20,
	([
	    SI_SKILLRESTR_LEARN:		([P_LEVEL: 15]),
	    SI_PRIM_ATTR:	A_INT,
	    SI_SKILLRESTR_USE:
		([SR_FREE_HANDS: 2]),
	    SI_SKILLDAMAGE_TYPE:	({DT_FIRE, DT_EXPLOSION}),
	    SI_SKILLDAMAGE:		300,
	    OFFSET(SI_SKILLDAMAGE):	 75,
	    SI_TIME_MSG:
	"Der Feuerball ist für das Werfen noch nicht stabil genug!\n",
	    SI_FAIL_MSG:
		"Der Feuerball verfehlt Dein Opfer ein gutes Stück und "+
		"löst sich in heiße Luft auf.\n",
	    SI_LEARN_MSG:
		"Du entwickelst eine Methode, den Feuerball besser zu "+
		"bündeln.\n",
	]));

    AddSpell("schnell", 40,
	([
	    SI_SKILLRESTR_LEARN:		([P_LEVEL: 17]),
	    SI_PRIM_ATTR:	A_CON,
	    SI_SECU_ATTR:	A_DEX,
	    SI_TIME_MSG:
		"Nicht so hastig!\n"
	]));



}


/* -- Jetzt kommen die Sprueche ------------------------------------------ */
int blitz(object caster, mapping sinfo)
{
    object env, ob;
    string str, txt;
    int		suc;

    if(environment(caster) &&
       environment(caster)->QueryProp(P_TOTAL_LIGHT) > 0)
    {
	write("Dafür ist es hier zu hell.\n");
	return 0;
    }

    if(caster->QueryProp(P_BLIND))
    {
        write(break_string(
	"Du bist zur Zeit geblendet und die Anwendung dieses Spruches "
        "hilft Dir da nicht viel... "));
        return 0;
    }

    suc=SpellSuccess(caster, sinfo);
    if(suc <= 0)
    {
	if(suc <= -4000)
	{
	    write(break_string(
	    "Irgendwie hast Du Dich mit Deiner Zauberei "
	    "übel vertan oder es übertrieben, denn auf einmal "
	"wird es gleißend hell vor Deinen Augen und Du blendest "
		"Dich selbst."));
	    caster->SetProp(P_BLIND,
		([BLIND_TIME:	4+(-(suc/500)),
		  BLIND_MSG:
	    "Deine Augen verschließen sich vor dem grellen Blitz.\n"
		]));
	}

	return MISSERFOLG;
    }

  env=environment(caster);
  if (str=sinfo[SI_SKILLARG])
  {
    // Reichen SP fuer Detail/Objekt-Blitzen?
    if (2>caster->QueryProp(P_SP))
      return write("Deine Konzentration reicht nicht aus, um Dich so "
                   "genau umzublicken.\n"), 0;

    // erhoehte Schwierigkeit fuer Detail-Blitzen, auch fuer Fortgeschrittene
    if((suc > 1000) || (random(sinfo[SI_SKILLABILITY])>1500))
    {
      // Jetzt gehts los
      if ((ob=present(str,env)) || (ob=present(str,caster)))
        if (!ob->QueryProp(P_INVIS) && ob->short() && txt=ob->long())
        {
          caster->restore_spell_points(-2);
          write("Du erkennst "+ob->name(WEN)+":\n"+txt);
          return ERFOLG;
        }
      if ((txt=env->GetReadDetail(str, caster->QueryProp(P_RACE))) ||
          (txt=env->GetDetail(str, caster->QueryProp(P_RACE))))
      {
        caster->restore_spell_points(-2);
        write(txt);
        return ERFOLG;
      }
      return write("Du findest leider nicht, was Du suchst.\n"), 0;
    }
    write("Du schaffst es nicht, Dich so genau zu konzentrieren.\n");
  }
  write("Für einen Moment erhascht Du einen Blick auf Deine Umgebung.\n");
  write(environment(caster)->int_long(caster, caster)
	  ||"Irgendwie scheint der Spruch zu schwach zu sein, als dass er "+
	    "die Umgebung genügend erhellt.\n");

    return ERFOLG;
}

int identifiziere(object caster, mapping sinfo)
{
    int		suc;
    object	ob;
    string	res, s;
    mapping	mat;
    int		c, h;
    string namef,shortf;
    int magicf;

    ob=FindVictim(sinfo[SI_SKILLARG],
		  caster, "Was möchtest Du identifizieren?\n");
    if(!ob) return 0;

    magicf=ob->QueryProp(P_MAGIC);
    namef=ob->Name(WER,1);
    shortf=ob->QueryProp(P_SHORT);

    suc=SpellSuccess(caster, sinfo);
    if(suc <= 0)
    {
	return MISSERFOLG;
    }

    // Versteckte Information?
    if (res=ob->info(caster) || ob->QueryProp(P_INFO))
      printf(VT_TA_B+"Du entreißt %s folgendes Geheimnis:"+VT_NORMAL+"\n%s\n",
             ob->name(WEM, 1), break_string(res,78," ",BS_LEAVE_MY_LFS));
    else
	printf("Du kannst nichts Besonderes an %s erkennen.\n",
	       ob->name(WEM, 1));

    /* class_info */

    suc+=3*caster->QueryAttribute(sinfo[SI_PRIM_ATTR]);
    suc+=2*caster->QueryAttribute(sinfo[SI_SECU_ATTR]);

    /* Temperatur */
    if(suc > 4000)
    {
	if (mappingp(mat=ob->QueryProp(P_TEMPERATURE_PROTECTION)))
	{
	    s=ob->Name(WER,1); c=mat[T_COLD_PROT]; h=mat[T_HEAT_PROT];
	    if(c)
	    {
		s+=" stellt bei Kälte ";
		if (c < -10) s+="eine enorme Belastung";
		else if (c < -5) s+="eine ziemliche Belastung";
		else if (c < -2) s+="eine spürbare Belastung";
		else if (c < -1) s+="eine leichte Belastung";
		else if (c < 0)  s+="eine geringe Belastung";
		else if (c > 10) s+="einen enormen Schutz";
		else if (c > 5)  s+="einen sehr guten Schutz";
		else if (c > 2)  s+="einen guten Schutz";
		else if (c > 1)  s+="einen ganz brauchbaren Schutz";
		else s+="einen eher geringen Schutz";
		s+=" dar"+(h ? " und" : "");
	    }

	    if(h)
	    {
		if(h < -10) s+=" belastet Dich"+(c<-10 ? " auch" : "")+
		               " bei Hitze enorm stark";
		else if (h < -5) s+=" bringt Dich bei Hitze schnell zum Schwitzen";
		else if (h < -2) s+=" lässt Dich bei Hitze leicht schwitzen";
		else if (h < -1) s+=" lässt Dich bei Hitze leichter schwitzen";
		else if (h < 0)  s+=" ist bei Hitze eine leichte Belastung für Dich";
		else if (h > 10) s+=" schützt Dich unglaublich gut vor Hitze";
		else if (h > 5)  s+=" bietet Dir einen beachtlichen Hitzeschutz";
		else if (h > 2)  s+=" schützt Dich recht gut vor Hitze";
		else if (h > 1)  s+=" schützt Dich einigermaßen vor Hitze";
		else s+=" schützt Dich ein wenig vor Hitze";
	    }

	    if (c || h)
	    {
		write(break_string(s+"."));
	    }
	}
    }


    /* Materialerkennung */
    if(suc > 5000)
    {
	if((mat=ob->QueryProp(P_MATERIAL)) && (pointerp(mat)))
	{
	// 'lebende Materie' bei NPCs kommt nicht so dolle, aber livings
	// sollte man schon identifizieren koennen, wg. enable_commands()
	// (Mupfel@AL)
	// Anm. Woody: Im SL haben wir MAT_LIVING (noch) nicht
	// if (living(ob)) mat -= ({MAT_LIVING});

	if(sizeof(mat))
	{
	    write(break_string(sprintf("%s besteh%s aus %s.",
		ob->Name(WER,1),
		(ob->IsUnit() && ob->QueryProp(P_REQUESTED_AMOUNT) != 1 ? "en" : "t"),
		ob->MatList(WEM) || "Dir unbekanntem Material")
	    ));
	}
	else if(mappingp(mat) && sizeof(mat-([MAT_MISC_LIVING]) ))
	{
	    write(break_string(sprintf("%s besteh%s aus %s.",
		ob->Name(WER,1),
		(ob->IsUnit() && ob->QueryProp(P_REQUESTED_AMOUNT) != 1 ? "en" : "t"),
		 ob->MaterialList(WEM, caster) || "Dir unbekanntem Material")
	    ));
	}
	}
    }

    return ERFOLG;
}


int schätz(object caster, mapping sinfo)
{
   object victim;
   int hpdiff, wcdiff, acdiff;
   int hp, vhp, wc, vwc, ac, vac, gen, val;
   string result;
   string vname, vnamegen;

   victim=FindVictim(sinfo[SI_SKILLARG],caster,
                           "Wen oder was willst Du denn einschätzen?\n");
   if (!victim)
     return 0;
   if (hp=SpellSuccess(caster,sinfo)<=0)
   {
     if(hp < -2500)
     {
     	write("Du verschätzt Dich total und wirkst ziemlich verwirrt.\n");
     	caster->SetProp(P_PARALYZED,
     		([PARALYZE_TIME: 10,
     		  PARALYZE_MSG: "Du bist noch wie gelähmt.\n",
     		  PARALYZE_MSG_OTHER: "%s schaut doof aus der Wäsche.\n",
     		  PARALYZE_END: "Du kannst wieder klarer denken.\n"
     		 ]));

     }


      return MISSERFOLG;
   }

   if (!living(victim))
   {
     result = sprintf("Du schätzt %s ab und kommst zu folgendem Schluß:\n",
                      victim->name(WEN));
     if (val = victim->QueryProp(P_VALUE))
       result += sprintf("%s ha%s einen Wert von %d Tametaler%s.\n",
                      capitalize(victim->name(WER,1)),
                      (victim->IsUnit() && victim->QueryProp(P_REQUESTED_AMOUNT) != 1) ?
                        "ben" : "t",
                      val, (val==1 ? "" : "n"));
     else
       result += sprintf("%s %s völlig wertlos...\n",
                      capitalize(victim->name(WER,1)),
                      (victim->IsUnit() && victim->QueryProp(P_REQUESTED_AMOUNT) != 1) ?
                        "sind" : "ist");

     if (victim->IsUnit())
       val = victim->QueryProp(P_WEIGHT);
     else
       val = victim->QueryProp(P_TOTAL_WEIGHT);

     // ab 6 kg nur mehr auf 1 kg genau
     if ( (val >= 6000) || (val > (3000+100*caster->QueryProp(P_LEVEL))) )
       vname = "in etwa "+((val+500)/1000)+" Kilogramm";
     else
     {
       if (val < 100)
         vname = "so gut wie nichts";
       else if (val < 950)
         vname = "schätzungsweise "+((val+50)-(val+50)%100)+" Gramm";
         // vname=sprintf("%.1f",to_float(val)/1000);
       else
       {
         if (val >= 1500)
           val = val+random(100)-random(100);   // ein wenig random :)
         if (val >= 2000)
           val=(val+250)-(val+250)%500;
         else
           val=(val+125)-(val+125)%250;
         vname = "ungefähr "+
           ({"","ein","zwei","drei","vier","fünf","sechs"})[val/1000]+
           ({"","einviertel","einhalb","dreiviertel"})[(val%1000)/250]+
           " Kilogramm";
       }
     }

     result += sprintf("%s wieg%s %s.\n",
         capitalize(victim->QueryPronoun()),
         (victim->IsUnit() &&
          victim->QueryProp(P_REQUESTED_AMOUNT) != 1) ? "en" : "t", vname);

     write (result);
     return ERFOLG;
   }

   hp = caster->QueryProp(P_HP);    vhp = victim->QueryProp(P_HP);
   wc = caster->QueryProp(P_TOTAL_WC);  vwc = victim->QueryProp(P_TOTAL_WC);
   ac = caster->QueryProp(P_TOTAL_AC);  vac = victim->QueryProp(P_TOTAL_AC);
   hpdiff = hp - vhp;
   wcdiff = wc - vwc;
   acdiff = ac - vac;
   vname = victim->name(WER, 1);
   vnamegen = victim->name(WESSEN, 1);
   gen = victim->QueryProp(P_GENDER);

   if (hpdiff > 2*vhp)
      result = "Du bist " + victim->name(WEM, 1) + " haushoch überlegen.\n";
   else if (hpdiff > vhp)
      result = "Du bist viel stärker als " + vname + ".\n";
   else if (hpdiff > vhp/5)
      result = "Du bist stärker als " + vname + ".\n";
   else if (-hpdiff > 2*hp)
      result = capitalize(vname) + " ist Dir haushoch überlegen.\n";
   else if (-hpdiff > hp)
      result = capitalize(vname) + " ist viel stärker als Du.\n";
   else if (-hpdiff > hp/5)
      result = capitalize(vname) + " ist stärker als Du.\n";
   else
      result = capitalize(vname) + " ist etwa gleich stark wie Du.\n";
   write(result);

   if (acdiff > 2*vac)
      result = "Deine Rüstung ist unglaublich viel besser als die "
     + vnamegen + ".\n";
   else if (acdiff > vac)
      result = "Deine Rüstung ist viel besser als die " + vnamegen + ".\n";
   else if (acdiff >vac/5)
      result = "Deine Rüstung ist besser als die " + vnamegen + ".\n";
   else if (-acdiff > 2*ac)
      result = "Die Rüstung " + vnamegen
     + " ist unglaublich viel besser als Deine.\n";
   else if (-acdiff > ac)
      result = "Die Rüstung " + vnamegen + " ist viel besser als Deine.\n";
   else if (-acdiff > ac/5)
      result = "Die Rüstung " + vnamegen + " ist besser als Deine.\n";
   else
      result = "Eure Rüstungen sind etwa gleich gut.\n";
   write(result);

   if (wcdiff > 2*vwc)
      result = "Deine Waffe ist eigentlich zu gut für diesen Gegner.\n";
   else if (wcdiff > vwc)
      result = "Deine Waffe ist viel besser als die " +vnamegen + ".\n";
   else if (wcdiff > vwc/5)
      result = "Deine Waffe ist besser als die " +vnamegen + ".\n";
   else if (-wcdiff > 2*wc)
      result = "Die Waffe " + vnamegen
     + " lässt Deine ganz mickrig aussehen.\n";
   else if (-wcdiff > wc)
      result = "Die Waffe " + vnamegen + " ist viel besser als Deine.\n";
   else if (-wcdiff > wc/5)
      result = "Die Waffe " + vnamegen + " ist besser als Deine.\n";
   else
      result = "Eure Waffen sind etwa gleich gut.\n";

  result += capitalize(victim->QueryPronoun(WER) || "es");
  if ((val = victim->QueryProp(P_ALIGN)) <= -1000)
    result += " ist von satanischer Gesinnung.\n";
  else if (val <= -200)
    result += " ist von böser Gesinnung.\n";
  else if (val <= -40)
    result += sprintf(" ist ein%s unfreundliche%s Zeitgenoss%s.\n",
        (gen == FEMALE ? "e" : ""), (gen == FEMALE ? "" : "r"),
        (gen == FEMALE ? "in" : "e") );
  else if (val <= 40)
    result += " sieht die Welt mit neutralen Augen.\n";
  else if (val <= 200)
    result += " ist eigentlich ganz nett.\n";
  else if (val < 1000)
    result += " ist von guter Gesinnung.\n";
  else
    result += " strahlt eine gewisse Heiligkeit aus.\n";

   write(result);
   return ERFOLG;
}

int pfeil(object caster, mapping sinfo)
{
    object	victim;
    int		hands, suc;
    string	vn, vnw, cn;


  victim=FindEnemyVictim(sinfo[SI_SKILLARG],caster,
                         "Nach wem willst Du einen magischen Pfeil werfen?\n");
  if (!victim) return 0;
  suc=SpellSuccess(caster, sinfo);
  if (suc <= 0)
  {
    if(suc < -3500)
    {
	write("Geschickterweise steuerst Du Dir den magischen Pfeil selber "+
	      "in Dein Bein.\n");
	caster->reduce_hitpoints(10+random(30));
    }

    return MISSERFOLG;
  }

  cn = capitalize(caster->name());
  vnw = victim->name(WEN, 2);
  say(sprintf("%s feuert einen magischen Pfeil auf %s ab.\n",cn, vnw),
      victim);
  printf("Du feuerst einen magischen Pfeil auf %s ab.\n", vnw);
  tell_object(victim,sprintf("%s feuert einen magischen Pfeil auf Dich ab.\n",
                             cn));

  if(random(10) > 5)
  {
        sinfo[SI_SKILLDAMAGE] =
	    sinfo[SI_SKILLDAMAGE] +
	    2*caster->QueryAttribute(sinfo[SI_PRIM_ATTR]) +
	    caster->QueryAttribute(sinfo[SI_SECU_ATTR]);
  }

  TryDefaultAttackSpell(victim,caster,sinfo,
                        ([SP_NAME:"Pfeil",SP_SHOW_DAMAGE:1]));
  return ERFOLG;
}

int feuerball(object caster, mapping sinfo)
{
  object victim;
  int hands, suc;
  string vnw, feuertext;

  victim=FindEnemyVictim(sinfo[SI_SKILLARG],caster,
                         "Auf wen willst Du einen Feuerball schleudern?\n");
  if (!victim)
  {  return 0; }

  suc=SpellSuccess(caster, sinfo);

  if(suc < 0)
  {
    if(suc < -1500)
    {
	write("Du kokelst Dir Deine Hände mit den Feuerball an.\n");
	write("AUUUUUUUUUAH!\n");
        SetProp(P_KILL_NAME,"Selbstentzündung");
	caster->reduce_hitpoints(20+random(15));
    }
    return MISSERFOLG;
  }

  suc+=5*caster->QueryAttribute(sinfo[SI_PRIM_ATTR])+
       4*caster->QueryAttribute(sinfo[SI_SECU_ATTR]);


  switch(suc)
  {
    case 0..2000:
	feuertext = "ein kleines Feuerbällchen";
	break;

    case 2001..4000:
	feuertext = "einen kleinen Feuerball";
	break;

    case 4001..5000:
	feuertext = "einen mittleren Feuerball";
	break;

    case 5001..7500:
	feuertext = "einen großen Feuerball";
	break;

    default:
	feuertext = "einen gewaltig großen Feuerball";
	break;
  }

  vnw = victim->name(WEM, 2);
  say(sprintf("%s wirft "+feuertext+" nach %s.\n",
              capitalize(caster->name()), vnw), victim);
  printf("Du wirfst "+feuertext+" nach %s.\n", vnw);
  tell_object(victim, sprintf("%s wirft "+feuertext+" nach Dir.\n",
                              capitalize(caster->name())));

  TryDefaultAttackSpell(victim,caster,sinfo,
                        ([SP_NAME:"Feuerball",SP_SHOW_DAMAGE:1]));
  return ERFOLG;
}

int schnell(object caster, mapping sinfo)
{
    int speed, dauer, suc;
    string speedtext;

    if(!caster->InFight())
    {
	write("Außerhalb eines Kampfes ist das ziemlich sinnlos...\n");
	return 0;
    }

    // norm == 100, aber zur Zeit gibts keine anderen Voelker mit
    // SA_SPEED != 100
    if(caster->QuerySkillAttribute(SA_SPEED) != 100)
    {
        write("Zur Zeit wirkt gerade etwas, was Deine Geschwindigkeit "+
              "beeinflusst.\n");
        return 0;
    }

    dauer = (caster->QueryAttribute(sinfo[SI_PRIM_ATTR]) +
             caster->QueryAttribute(sinfo[SI_SECU_ATTR])/2 +
	     random(sinfo[SI_SKILLABILITY]/1000));
    //dauer = (dauer*6)/10;

    suc=SpellSuccess(caster,sinfo);
    if(suc <= 0)
    {

	if(suc < -2000)
	{
	 write("Der Spruch ging total daneben! Alles wird so zähhh...\n");
	    caster->ModifySkillAttribute(caster, SA_SPEED, 50, dauer);
	}

	return MISSERFOLG;
    }


    suc+=3*caster->QueryAttribute(sinfo[SI_PRIM_ATTR])+
         2*caster->QueryAttribute(sinfo[SI_SECU_ATTR]);


    speed=200;
    speedtext="um einiges schneller";
    if(suc > 8500)
    {
	speed=300+caster->QueryAttribute(sinfo[SI_PRIM_ATTR]);
	speedtext="einfach ultraschnell";
    }
    else if(suc > 8000)
    {
	speed=280+caster->QueryAttribute(sinfo[SI_PRIM_ATTR]);
	speedtext="wahnsinnig schnell";
    }
    else if(suc > 7000)
    {
	speed=250;
	speedtext="sehr schnell";
    }
    caster->ModifySkillAttribute(caster, SA_SPEED, speed, dauer);

   write("Du fühlst Dich plötzlich "+speedtext+" in Deinen Bewegungen!\n");
    //ADB(sprintf("SCHNELL %s: suc=%i, dauer=%i, speed=%i",
    //    getuid(caster), suc, dauer, speed));

    return ERFOLG;
}


