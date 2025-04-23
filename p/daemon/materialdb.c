// Tamedhon MUDlib
//
// p/daemon/materialdb.c -- Materialdatenbank
//
// $Date: 2004/01/28 17:10:00 $
/* $Revision: 1.3 $
 * $Log: materialdb.c,v $
 * Revision 1.3  2004/01/28 17:10:00  Serii
 * neues material: harz
 * ausserdem is die matdb jetzt kein 'ding' mehr :)
 *
 * Revision 1.2  1998/01/20 15:42:03  Feyaria
 * neue materialien, matgroup_swimable, mat_knowlege
 *
 * Revision 1.1  1997/09/27 14:45:54  Woody
 * Initial revision
 *
 */

//inherit "/std/thing"; //Serii

#include <thing/material.h>
#include <properties.h>
#include <language.h>
#define PROP_RECOC "material_recognizability"
#define EXCLUDE_ARRAY ({P_NAME,P_GENDER,PROP_RECOC})
  
#pragma strong_types

// Prototypes:
int MaterialGroup(mapping mats, string grp);
// (Liefert den Anteil der Materialgruppe grp an mats)
varargs string ConvMaterialList(mixed mats, int casus, mixed idinf);
varargs string MaterialName(string mat, int casus, mixed idinf);
string GroupName(string grp);
string *AllMaterials();
string *AllGroups();
string *GetMatMembership(string mat);
string *GetGroupMembers(string grp);
void Dump();
// (Gibt Liste aller Materialien und Gruppen in /p/daemon/save/MATERIALS aus)

static mapping materials;
static mapping material_groups;

static varargs void AddMaterial(string mat, int gender,
					mixed names, mixed groups,
					mixed dif) {
  mixed *addgrps, *grps, ind;
  mapping props;
  int i,j,val;
  string s;
  
  if (!pointerp(names))
    names=({names});
  if (sizeof(names)<1)
    names=({capitalize(mat)});
  if (sizeof(names)<2)
    names+=({names[0]+"s"});
  if (sizeof(names)<3)
    names+=({names[0]});
  if (sizeof(names)<4)
    names+=({names[0]});
  props=([P_GENDER:gender,
	  P_NAME:names]);
  
  if (mappingp(groups)) {
    props+=groups;
  } else if (pointerp(groups)) {
    for (i=sizeof(groups)-1;i>=0;i--)
      props[groups[i]]=100;
  } else if (stringp(groups)) {
    props[groups]=100;
  }

  // Bei schwer zu erkennenden Materialien ungenauere Beschreibungen festlegen
  if (pointerp(dif)) {
    if (sizeof(dif)%2!=1)
      raise_error("materialdb: illegal unprecise description array\n");
    props[PROP_RECOC]=dif;
  } else if (dif && intp(dif)) {
    if (stringp(groups))
      s=groups[3..];
    else if (pointerp(groups) && sizeof(groups)>=1)
      s=groups[0][3..];
    else
      s="?";
    if (materials[s])
      props[PROP_RECOC]=({s,dif,mat});
    else if (materials["misc_"+s])
      props[PROP_RECOC]=({"misc_"+s,dif,mat});
    else
      raise_error(sprintf("materialdb: no unprecise description "+
			  "for %s found\n",mat));
  }
  
  // Alles, was nicht Gas oder Fluessigkeit ist, ist fest:
  if (!member(props,MATGROUP_FLUID)
      && !member(props,MATGROUP_GAS)
      && !member(props,MATGROUP_SOLID))
    props[MATGROUP_SOLID]=100;
  addgrps=({ // Reihenfolge wird rueckwaerts abgearbeitet
    // Schwimmende Sachen
    ({MATGROUP_SWIMABLE, MATGROUP_PAPER, MATGROUP_CLOTH, MATGROUP_HERBAL,
      MATGROUP_FLUID, MATGROUP_GAS}),
    // Papier und Stoff sind biegsam:
    ({MATGROUP_FLEXIBLE, MATGROUP_CLOTH, MATGROUP_PAPER}),
    // Gase sieht man normalerweise nicht:
    ({MATGROUP_INVIS, MATGROUP_GAS}),
    // Saeuren sind immer schlecht zu trinken:
    ({MATGROUP_POISONOUS,MATGROUP_ACIDIC}),
    // Mineralien sind auch Steine
    ({MATGROUP_STONE,MATGROUP_MINERAL}),
    // Edelsteine sind Mineralien
    ({MATGROUP_MINERAL,MATGROUP_JEWEL}),
    // Edelmetalle sind Metalle:
    ({MATGROUP_METAL,MATGROUP_PRECIOUS_METAL}),
    // Lebewesen und deren Ueberreste sind biologisch:
    ({MATGROUP_BIO,MATGROUP_LIVING,MATGROUP_DEAD}),
    // Holz ist pflanzlich:
    ({MATGROUP_HERBAL, MATGROUP_WOOD}),
    // Holz ist meistens tot:
    ({MATGROUP_DEAD,MATGROUP_WOOD}),
    // Holz und Papier brennt:
    ({MATGROUP_INFLAMMABLE,MATGROUP_WOOD,MATGROUP_PAPER}),
    // Laubhoelzer, Nadelhoelzer und Tropenhoelzer sind Holz
    ({MATGROUP_WOOD,MATGROUP_TROPICAL_WOOD}),
    ({MATGROUP_WOOD,MATGROUP_DECIDUOUS_WOOD,MATGROUP_CONIFER_WOOD}),
    // Explosive Dinge sind immer entzuendlich:
    ({MATGROUP_INFLAMMABLE,MATGROUP_EXPLOSIVE})
  });
  for (i=sizeof(addgrps)-1;i>=0;i--) {
    grps=addgrps[i];
    if (member(props,grps[0])) // Existiert schon eigener Eintrag?
      continue; // Automatische Eintragung unnoetig
    val=0;
    for (j=sizeof(grps)-1;j>=1;j--)
      val+=props[grps[j]];
    if (!val)
      continue;
    if (val>100)
      val=100;
    if (val<-100)
      val=-100;
    props[grps[0]]=val;
  }
  if (props[MATGROUP_LIVING]) // Im Falle von lebendem Holz, tot loeschen
    efun::m_delete(props,MATGROUP_DEAD);

  addgrps=m_indices(props)-EXCLUDE_ARRAY;
  for (i=sizeof(addgrps)-1;i>=0;i--) {
    ind=addgrps[i];
    if (!props[ind]) {
      efun::m_delete(props,ind);
      continue;
    }
    if (!pointerp(grps=material_groups[ind]))
      grps=({});
    grps+=({mat});
    material_groups[ind]=grps;
  }
  
  materials[mat]=props;
}

void create() {
  if (clonep(this_object()))
    return destruct(this_object());
//  ::create(); //Serii
  materials=([]);
  material_groups=([]);
  
  // Feste Materialien:
  AddMaterial(MAT_MISC_SOLID,NEUTER,
	      ({"verschiedenes Material","verschiedenen Materials",
                "verschiedenem Material","verschiedenen Material"}));
  AddMaterial(MAT_GLASS, NEUTER, ({"Glas", "Glasses"}));
  AddMaterial(MAT_CERAMIC, FEMALE, ({"Keramik", "Keramik"}));
  AddMaterial(MAT_ICE, NEUTER, ({"Eis", "Eises"}),MATGROUP_SWIMABLE);
  AddMaterial(MAT_PHOSPHORUS, MALE, "Phosphor",MATGROUP_POISONOUS,
              ({MAT_MISC_SOLID,-5,MAT_PHOSPHORUS}));
  AddMaterial(MAT_CARBON, MALE, "Kohlenstoff",MATGROUP_INFLAMMABLE,
              ({MAT_MISC_SOLID,-5,MAT_CARBON}));
  AddMaterial(MAT_SALPETER,MALE,"Salpeter", 0, 
              ({MAT_MISC_SOLID,-5,MAT_SALPETER}));
  AddMaterial(MAT_TAR, MALE, "Teer",0, ({MAT_MISC_SOLID,-5,MAT_TAR}));
  AddMaterial(MAT_WAX, NEUTER, ({"Wachs","Wachses"}), 0,
              ({MAT_MISC_SOLID,-5,MAT_WAX}));
  AddMaterial(MAT_CHALK, FEMALE, ({"Kreide","Kreide"}), 0, 
              ({MAT_MISC_SOLID,-10,MAT_CHALK}));
  AddMaterial(MAT_SHIT, FEMALE, ({"Scheiße","Scheiße"}));
  AddMaterial(MAT_FAT, NEUTER, "Fett", 
              ({MATGROUP_INFLAMMABLE,MATGROUP_SWIMABLE}),
              ({MAT_MISC_SOLID,-15,MAT_FAT}));
  AddMaterial(MAT_RESIN, NEUTER, ({"Harz","Harzes"}),MATGROUP_HERBAL);
  AddMaterial(MAT_PEARL, NEUTER, "Perlmutt",0,
              ({MAT_MISC_SOLID,-10,MAT_PEARL}));
  AddMaterial(MAT_PLASTIC, NEUTER, 
	      ({"künstliches Material", "künstlichen Materials",
                "künstlichem Material", "künstlichen Material"}), 0,
	      ({MAT_MISC_SOLID,35,MAT_PLASTIC}));

  AddMaterial(MAT_PAPER, NEUTER, "Papier", MATGROUP_PAPER);
  AddMaterial(MAT_PERGAMENT, NEUTER, "Pergament", MATGROUP_PAPER,-20);
  AddMaterial(MAT_PAPYRUS, MALE, "Papyrus", MATGROUP_PAPER,-15);
  AddMaterial(MAT_CARTON,FEMALE,({"Pappe","Pappe"}),
	      ([MATGROUP_PAPER:100,MATGROUP_FLEXIBLE:0]),
              ({MAT_PAPER,-20,MAT_CARTON}) );
  
  AddMaterial(MAT_CLOTH, MALE, ({"Stoff", "Stoffes"}),MATGROUP_CLOTH);
  AddMaterial(MAT_SILK, FEMALE, ({"Seide","Seide"}), MATGROUP_CLOTH,-10);
  AddMaterial(MAT_VELVET, MALE, "Samt", MATGROUP_CLOTH,-15);
  AddMaterial(MAT_COTTON, FEMALE, ({"Baumwolle","Baumwolle"}), 
              MATGROUP_CLOTH, -10);
  AddMaterial(MAT_FELT, MALE, ({ "Filz", "Filzes" }), MATGROUP_CLOTH,-10);
  AddMaterial(MAT_BAUSCH, MALE, ({ "Bausch", "Bausches" }), MATGROUP_CLOTH,10);
  
  AddMaterial(MAT_EARTH, FEMALE, ({"Erde","Erde"}), MATGROUP_ELEMENTAL);
  AddMaterial(MAT_SAND, MALE, "Sand");
  AddMaterial(MAT_DUST, MALE, "Staub");
  AddMaterial(MAT_PLUTONIUM, NEUTER, "Plutonium",
	      ({MATGROUP_POISONOUS, MATGROUP_RADIOACTIVE}),
	      ({MAT_DUST,65,MAT_PLUTONIUM}));
  
  // Hoelzer:
  AddMaterial(MAT_MISC_WOOD, NEUTER,({"Holz", "Holzes"}), MATGROUP_WOOD);
  AddMaterial(MAT_MISC_DECIDUOUS_WOOD, NEUTER,({"Laubholz", "Laubholzes"}),
	      MATGROUP_DECIDUOUS_WOOD);
  AddMaterial(MAT_MISC_CONIFER_WOOD, NEUTER,({"Nadelholz", "Nadelholzes"}),
	      MATGROUP_CONIFER_WOOD);
  AddMaterial(MAT_MISC_TROPICAL_WOOD, NEUTER,({"Tropenholz", "Tropenholzes"}),
	      MATGROUP_TROPICAL_WOOD);
  AddMaterial(MAT_OAK,NEUTER,({"Eichenholz","Eichenholzes"}),
	      MATGROUP_DECIDUOUS_WOOD,-15);
  AddMaterial(MAT_EBONY,NEUTER,({"Ebenholz","Ebenholzes"}),
	      MATGROUP_DECIDUOUS_WOOD,15);
  AddMaterial(MAT_BEECH,NEUTER,({"Buchenholz","Buchenholzes"}),
	      MATGROUP_DECIDUOUS_WOOD,-5);
  AddMaterial(MAT_FIR,NEUTER,({"Tannenholz","Tannenholzes"}),
	      MATGROUP_CONIFER_WOOD,15);
  AddMaterial(MAT_PEAR_WOOD, NEUTER,
	      ({"Birnbaumholz", "Birnbaumholzes"}),
	      ({MATGROUP_DECIDUOUS_WOOD,MATGROUP_MAGIC}),35);
  AddMaterial(MAT_LUMINARA_WOOD,NEUTER,
              ({"Luminaraholz","Luminaraholzes"}),
	      ({MATGROUP_DECIDUOUS_WOOD,MATGROUP_MAGIC}),45);
  AddMaterial(MAT_TEAK,NEUTER,({"Teakholz","Teakholzes"}),
	      MATGROUP_TROPICAL_WOOD,25);
  AddMaterial(MAT_CORK,MALE,"Kork",MATGROUP_WOOD,-5);
  AddMaterial(MAT_BALSA,NEUTER,({"Balsaholz","Balsaholzes"}),MATGROUP_WOOD,-10);
  AddMaterial(MAT_BIRCH_WOOD,NEUTER,({"Birkenholz","Birkenholzes"}),
	      MATGROUP_DECIDUOUS_WOOD,-5);
  AddMaterial(MAT_ELM_WOOD,NEUTER,({"Ulmenholz","Ulmenholzes"}),
	      MATGROUP_DECIDUOUS_WOOD,10);
  AddMaterial(MAT_YEW_WOOD,NEUTER,({"Eibenholz","Eibenholzes"}),
	      MATGROUP_DECIDUOUS_WOOD,15);
  AddMaterial(MAT_ASH_WOOD,NEUTER,({"Eschenholz","Eschenholzes"}),
	      MATGROUP_DECIDUOUS_WOOD,20);
  AddMaterial(MAT_TICTOC_WOOD,NEUTER,({"Tictocholz","Tictocholzes"}),
	      MATGROUP_TROPICAL_WOOD,25);
  
  // Steine:
  AddMaterial(MAT_MISC_STONE, MALE, ({"Stein", "Steines"}), MATGROUP_STONE);
  AddMaterial(MAT_GRANITE, MALE, "Granit", MATGROUP_STONE,5);
  AddMaterial(MAT_SLATE, MALE, "Schiefer", MATGROUP_STONE,
              ({MAT_MISC_STONE,-5,MAT_SLATE}) );
  AddMaterial(MAT_MARBLE, MALE, "Marmor", MATGROUP_STONE,
              ({MAT_MISC_STONE,-25,MAT_MARBLE}) );
  AddMaterial(MAT_CONCRETE, MALE, "Beton", MATGROUP_STONE,5);
  AddMaterial(MAT_ASPHALT, MALE, "Asphalt", MATGROUP_STONE,10);
  AddMaterial(MAT_LIME, MALE, ({"Kalk", "Kalkes"}), MATGROUP_STONE,
              ({MAT_MISC_STONE,-10,MAT_LIME}) );
  
  // Metalle:
  AddMaterial(MAT_MISC_METAL, NEUTER, "Metall", MATGROUP_METAL);
  AddMaterial(MAT_IRON, NEUTER, "Eisen",
	      ({MATGROUP_METAL, MATGROUP_MAGNETIC}));
  AddMaterial(MAT_STEEL, NEUTER, "Stahl", MATGROUP_METAL,
              ({MAT_IRON,-10,MAT_STEEL}));
  AddMaterial(MAT_COPPER, NEUTER, "Kupfer", MATGROUP_METAL);
  AddMaterial(MAT_BRONCE, FEMALE, ({"Bronze", "Bronze"}), MATGROUP_METAL);
  AddMaterial(MAT_BRASS, NEUTER, "Messing", MATGROUP_METAL,
	      ({MAT_GOLD,-15,MAT_BRASS}));
  AddMaterial(MAT_TIN, NEUTER, "Zinn", MATGROUP_METAL,
	      ({MAT_SILVER,-25,MAT_TIN}));
  AddMaterial(MAT_TITANIUM, NEUTER, "Titan", MATGROUP_METAL,15);
  AddMaterial(MAT_LEAD, NEUTER, "Blei",
	      ({MATGROUP_METAL,MATGROUP_POISONOUS,MATGROUP_FLEXIBLE}));
  AddMaterial(MAT_MERCURY, NEUTER, "Quecksilber",
	      ({MATGROUP_METAL,MATGROUP_POISONOUS,MATGROUP_FLUID}),10);
  AddMaterial(MAT_ALUMINIUM, NEUTER, "Aluminium",MATGROUP_METAL);
  AddMaterial(MAT_TUNGSTEN, NEUTER, "Wolfram", MATGROUP_METAL, 25);
  AddMaterial(MAT_COBALT, NEUTER, "Kobalt",
	      ({MATGROUP_METAL, MATGROUP_MAGNETIC}), 25);
  AddMaterial(MAT_IRIDIUM, NEUTER, "Iridium",
	      ({MATGROUP_METAL, MATGROUP_POISONOUS}), 35);
  AddMaterial(MAT_CHROMIUM, NEUTER, "Chrom", MATGROUP_METAL, 5);
  // Edelmetalle:
  AddMaterial(MAT_SILVER, NEUTER, "Silber", MATGROUP_PRECIOUS_METAL);
  AddMaterial(MAT_GOLD, NEUTER, ({"Gold", "Goldes"}), MATGROUP_PRECIOUS_METAL);
  AddMaterial(MAT_PLATINUM, NEUTER, "Platin", MATGROUP_PRECIOUS_METAL,
	      ({MAT_SILVER,-5,MAT_PLATINUM}));
  AddMaterial(MAT_MITHRIL, NEUTER, "Mithril", MATGROUP_PRECIOUS_METAL,
              ({MAT_MISC_METAL,-5,MAT_MITHRIL}) );
  AddMaterial(MAT_ADAMANT, NEUTER, "Adamant", MATGROUP_PRECIOUS_METAL,
	      ({MAT_MISC_METAL,10,MAT_ADAMANT}));


  // Fluessigkeiten:
  AddMaterial(MAT_MISC_FLUID, FEMALE, "Flüssigkeit", MATGROUP_FLUID);
  AddMaterial(MAT_WATER, NEUTER, "Wasser",
	      ({MATGROUP_FLUID,MATGROUP_ELEMENTAL}));
  AddMaterial(MAT_ALCOHOL, MALE, "Alkohol",
	      ({MATGROUP_FLUID, MATGROUP_INFLAMMABLE}));
  AddMaterial(MAT_OIL, NEUTER, "Öl",
	      ({MATGROUP_FLUID, MATGROUP_INFLAMMABLE}),
              ({MAT_MISC_FLUID,-10,MAT_OIL}) );
  AddMaterial(MAT_MISC_ACID, FEMALE, ({"Säure", "Säure"}),
	      ({MATGROUP_FLUID, MATGROUP_ACIDIC}));
  AddMaterial(MAT_BLOOD, NEUTER, "Blut",
	      ({MATGROUP_FLUID, MATGROUP_DEAD}));
  AddMaterial(MAT_PARFUM, NEUTER, "Parfüm", MATGROUP_FLUID,
              ({MAT_MISC_FLUID,-20,MAT_PARFUM}) );
  AddMaterial(MAT_MILK, FEMALE, ({"Milch","Milch"}), MATGROUP_FLUID);
  AddMaterial(MAT_INK, FEMALE, ({"Tinte","Tinte"}), MATGROUP_FLUID);
  AddMaterial(MAT_SLIME, MALE, "Schleim", MATGROUP_FLUID);
  
  // Gase:
  AddMaterial(MAT_MISC_GAS, NEUTER, ({"Gas","Gases"}), MATGROUP_GAS);
  AddMaterial(MAT_AIR, FEMALE, ({"Luft","Luft"}),
	      ({MATGROUP_GAS,MATGROUP_ELEMENTAL}));
  AddMaterial(MAT_METHANE, NEUTER, "Methan",
	      ({MATGROUP_GAS,MATGROUP_EXPLOSIVE}),20);
  AddMaterial(MAT_HELIUM, NEUTER, "Helium", MATGROUP_GAS,20);
  AddMaterial(MAT_LAUGHING_GAS, NEUTER,
	      ({"Lachgas","Lachgases"}), MATGROUP_GAS,
              ({MAT_MISC_GAS,-10,MAT_LAUGHING_GAS}) );

  // Edelsteine:
  AddMaterial(MAT_MISC_JEWEL, MALE, "Edelstein", MATGROUP_JEWEL);
  AddMaterial(MAT_OCTARINE, MALE, "Oktarin",
	      ({MATGROUP_JEWEL,MATGROUP_MAGIC}),5);
  AddMaterial(MAT_DIAMOND, MALE, "Diamant", MATGROUP_JEWEL, -30);
  // Oxide:
  AddMaterial(MAT_RUBY, MALE, "Rubin", MATGROUP_JEWEL,-20); // Corundum
  AddMaterial(MAT_SAPPHIRE, MALE, "Saphir", MATGROUP_JEWEL,-15); // Corundum
  // Silicate:
  AddMaterial(MAT_AQUAMARINE, MALE, "Aquamarin", MATGROUP_JEWEL,20); // Beryl
  AddMaterial(MAT_EMERALD, MALE, "Smaragd", MATGROUP_JEWEL,-15); // Beryl
  AddMaterial(MAT_JADE, FEMALE, ({"Jade","Jade"}), MATGROUP_JEWEL,-10);
  AddMaterial(MAT_AMETHYST, MALE, "Amethyst", MATGROUP_JEWEL,20); // Quarz
  AddMaterial(MAT_TOPAZ, MALE, "Topas", MATGROUP_JEWEL,20);
  // Mineraloide:
  AddMaterial(MAT_AMBER, MALE, "Bernstein", ({MATGROUP_DEAD,MATGROUP_JEWEL}),
              ({MAT_MISC_JEWEL,-10,MAT_AMBER}) );
  AddMaterial(MAT_OPAL, MALE, "Opal", MATGROUP_JEWEL,-5);

  // Mineralien:
  AddMaterial(MAT_MISC_MINERAL, NEUTER, "Mineral", MATGROUP_MINERAL);
  AddMaterial(MAT_CRYSTAL, MALE, "Kristall", MATGROUP_MINERAL,-5);
  AddMaterial(MAT_SULFUR, MALE, "Schwefel", MATGROUP_MINERAL,-25);
  // Silicate:
  AddMaterial(MAT_ASBESTOS, MALE, "Asbest",
	      ({MATGROUP_MINERAL,MATGROUP_POISONOUS}),
              ({MAT_MISC_MINERAL,15,MAT_ASBESTOS}) );
  AddMaterial(MAT_QUARTZ, MALE, "Quarz", MATGROUP_MINERAL,-10);
  AddMaterial(MAT_AGATE, MALE, "Achat", MATGROUP_MINERAL,
	      ({MAT_QUARTZ,20,MAT_AGATE})); // Quarz
  AddMaterial(MAT_LAZURITE, MALE, "Lasurit", MATGROUP_MINERAL,20);
  // Mineraloide:
  AddMaterial(MAT_OBSIDIAN, NEUTER, "Obsidian", MATGROUP_MINERAL,10);

  // pflanzliche Dinge:
  AddMaterial(MAT_MISC_HERBAL, NEUTER,
	      ({"pflanzliches Material", "pflanzlichen Materials",
                "pflanzlichem Material", "pflanzlichen Material"}),
	      MATGROUP_HERBAL);
  AddMaterial(MAT_MISC_PLANT, FEMALE, ({"Pflanze","Pflanze"}),
	      ({MATGROUP_HERBAL,MATGROUP_LIVING}));
  // (Pflanzen ohne Holzanteil)
  AddMaterial(MAT_WOODEN_PLANT, MALE, ({"Baum", "Baumes"}),
	      ({MATGROUP_HERBAL,MATGROUP_LIVING,MATGROUP_WOOD}));
  // (Pflanzen mit Holzanteil (Baeume, Gebuesche))
  AddMaterial(MAT_STRAW, NEUTER, "Stroh",
	      ({MATGROUP_HERBAL, MATGROUP_INFLAMMABLE, MATGROUP_FLEXIBLE}));

  // Drogen und Heiltraenke:
  AddMaterial(MAT_MISC_DRUG, FEMALE, ({"Droge","Droge"}), MATGROUP_DRUG);
  AddMaterial(MAT_CANNABIS, MALE, "Hanf",
	      ({MATGROUP_DRUG, MATGROUP_HERBAL, MATGROUP_FLEXIBLE}),
              ({MAT_MISC_HERBAL,20,MAT_CANNABIS}) );
  AddMaterial(MAT_HEAL, NEUTER, "Heilmittel", MATGROUP_DRUG);
  
  // Essbare Sachen
  AddMaterial(MAT_MISC_FOOD, NEUTER, "Essen", MATGROUP_EATABLE);
  AddMaterial(MAT_FRUIT, FEMALE, ({"Frucht","Frucht"}), MATGROUP_EATABLE,
              ({MAT_MISC_FOOD,-5,MAT_FRUIT}) );
  AddMaterial(MAT_SUGAR, MALE, "Zucker", MATGROUP_EATABLE,
              ({MAT_MISC_FOOD,-5,MAT_SUGAR}) );
  AddMaterial(MAT_BREAD, NEUTER, "Brot", MATGROUP_EATABLE,
              ({MAT_MISC_FOOD,-5,MAT_BREAD}) );
  AddMaterial(MAT_MUSHROOM, MALE, ({"Pilz", "Pilzes"}), MATGROUP_EATABLE,
              ({MAT_MISC_FOOD,-5,MAT_MUSHROOM}) );
  
  // Gewuerze
  AddMaterial(MAT_MISC_SPICE, NEUTER, ({"Gewürz","Gewürzes"}), 
              MATGROUP_EATABLE);
  AddMaterial(MAT_PEPPER, MALE, "Pfeffer", MATGROUP_EATABLE,
              ({MAT_MISC_SPICE,10,MAT_PEPPER}));
  AddMaterial(MAT_SALT, MALE, ({"Salz","Salzes"}), MATGROUP_EATABLE,
              ({MAT_MISC_SPICE,-15,MAT_SALT}));
  
  // Reste von Lebewesen:
  AddMaterial(MAT_MISC_DEAD, MALE, ({"sterbliche Überreste",
              "sterblicher Überreste", "sterblichen Überresten",
              "sterbliche Überreste"}), MATGROUP_DEAD);
  // (Alle festen toten Dinge (Knochen, Elfenbein, Leichen) die nicht speziell
  //  vorgesehen sind (Horn))
  AddMaterial(MAT_HORN, NEUTER, "Horn", MATGROUP_DEAD);
  AddMaterial(MAT_HAIR, FEMALE, ({"Haare","Haare","Haaren","Haaren"}),
	      ({MATGROUP_DEAD,MATGROUP_FLEXIBLE,MATGROUP_SWIMABLE}));
  AddMaterial(MAT_BONE, MALE, "Knochen", MATGROUP_DEAD);
  AddMaterial(MAT_CHITIN, NEUTER, "Chitin", MATGROUP_DEAD);
  AddMaterial(MAT_GHOST, MALE, "Geist",
              ({MATGROUP_DEAD, MATGROUP_GAS,MATGROUP_SWIMABLE}));
  AddMaterial(MAT_SKIN, FEMALE, ({"Haut","Haut"}),
	      ({MATGROUP_DEAD,MATGROUP_FLEXIBLE}));
  AddMaterial(MAT_FEATHER, FEMALE, ({"Federn","Federn"}),
	      ({MATGROUP_DEAD,MATGROUP_FLEXIBLE,MATGROUP_SWIMABLE}));
  AddMaterial(MAT_PELT, MALE, ({"Fell","Felles"}),
	      ({MATGROUP_DEAD, MATGROUP_FLEXIBLE}));
  AddMaterial(MAT_IVORY, NEUTER, "Elfenbein", MATGROUP_DEAD, -10);
  AddMaterial(MAT_WOOL, FEMALE, ({"Wolle","Wolle"}),
	      ({MATGROUP_DEAD, MATGROUP_FLEXIBLE,MATGROUP_SWIMABLE}));
  AddMaterial(MAT_LEATHER, NEUTER, "Leder",
	      ({MATGROUP_DEAD, MATGROUP_FLEXIBLE}));
  AddMaterial(MAT_FLESH, NEUTER, ({"Fleisch", "Fleisches"}),
	      ({MATGROUP_DEAD,MATGROUP_EATABLE}));

  // Sprengstoffe:
  AddMaterial(MAT_MISC_EXPLOSIVE, NEUTER,
	      ({"explosives Material", "explosiven Materials",
                "explosivem Material", "explosiven Material"}),
	      ([MATGROUP_SOLID:0,MATGROUP_EXPLOSIVE:100]));
  AddMaterial(MAT_GUNPOWDER,NEUTER,"Schwarzpulver",MATGROUP_EXPLOSIVE,-15);
  AddMaterial(MAT_NITROGLYCERINE,NEUTER,"Nitroglycerin",MATGROUP_EXPLOSIVE,-5);
  AddMaterial(MAT_TNT,NEUTER,"TNT",MATGROUP_EXPLOSIVE,-10);

  // Spezielle Materialien:
  AddMaterial(MAT_MISC,NEUTER, // Passt noch nicht einmal in Kategorie "fest"
	      ({"verschiedenes Material","verschiedenen Materials",
                "verschiedenem Material","verschiedenen Material"}),
	      ([MATGROUP_SOLID:0]));
  AddMaterial(MAT_MISC_MAGIC, FEMALE, ({"Magie", "Magie"}),
	      ([MATGROUP_SOLID:0, MATGROUP_MAGIC:100]));
  // (Fuer magische Gegenstaende, die nicht aus an sich schon
  //  magischen Materialien (Oktarin, Birnbaumholz) bestehen)
  AddMaterial(MAT_MISC_LIVING, NEUTER, "Lebewesen",MATGROUP_LIVING);
  // (bewegliche Lebewesen)
  AddMaterial(MAT_MISC_POISON, NEUTER,
	      ({"giftiges Material", "giftigen Materials",
                "giftigem Material", "giftigen Material"}),
	      ([MATGROUP_SOLID:0,MATGROUP_POISONOUS:100]));
  AddMaterial(MAT_MISC_INVIS, FEMALE, ({"", ""}),
	      ([MATGROUP_SOLID:0, MATGROUP_INVIS:100]));
  AddMaterial(MAT_MISC_FLEXIBLE, NEUTER,
	      ({"biegsames Material", "biegsamen Materials",
                "biegsamem Material", "biegsamen Material"}),
	      MATGROUP_FLEXIBLE);
  AddMaterial(MAT_MISC_MAGNETIC, NEUTER,
	      ({"magnetisches Material", "magnetischen Materials",
                "magnetischem Material", "magnetischen Material"}),
	      MATGROUP_MAGNETIC);
  AddMaterial(MAT_MISC_HOLY, NEUTER,
	      ({"heiliges Material", "heiligen Materials",
                "heiligem Material", "heiligen Material"}),
	      MATGROUP_HOLY);
  AddMaterial(MAT_MISC_UNHOLY, NEUTER,
	      ({"unheiliges Material", "unheiligen Materials",
                "unheiligem Material", "unheiligen Material"}),
	      MATGROUP_UNHOLY);
  AddMaterial(MAT_FIRE, NEUTER, "Feuer",
	      ({MATGROUP_ELEMENTAL, MATGROUP_GAS}));
  AddMaterial(MAT_LIGHTNING, MALE, ({"Blitz","Blitzes"}),
	      ({MATGROUP_GAS,MATGROUP_ELECTRICAL}));
  AddMaterial(MAT_SHADOW, MALE, "Schatten",
	      ([MATGROUP_SOLID:0]));
}

varargs string MaterialName(string mat, int casus, mixed idinf) {
  string *names;
  mapping props;
  mixed *dif, *grps, tmp, x;
  int i,j,recval;
  
  if (!mappingp(props=materials[mat]))
    props=([]);

  // Je nach Koennen des Spielers kann man das exakte Material
  // mehr oder weniger gut erkennen:
  if (pointerp(dif=props[PROP_RECOC])
      && (!intp(idinf)||idinf<100) ) { // 100=exakte Erkennung
    recval=0;
    grps=m_indices(props)-EXCLUDE_ARRAY;
    if (!pointerp(idinf))
      idinf=({idinf});

    // Zunaechst die Faehigkeit des Spielers (da koennen noch
    // Gildenfaehigkeiten hinzu kommen) ermitteln, dieses
    // Material zu erkennen:
    for (i=sizeof(idinf)-1;i>=0;i--) {
      tmp=idinf[i];
      if (objectp(tmp)) // Diese Property ist hauptsaechlich fuer Rassen:
	tmp=tmp->QueryProp(P_MATERIAL_KNOWLEDGE);
      if (intp(tmp)) {
	recval+=tmp; // Allgemeine Erkennungsfaehigkeit
	break;
      }
      if (closurep(tmp) && intp(x=funcall(tmp,mat,grps))) {
	recval+=x;
	break; // Closures koennen immer nuetzlich sein :)
      }
      if (mappingp(tmp)) {
	if ((x=tmp[mat]) && intp(x)){ // Erkennung von speziell diesem Material
	  recval+=x;
	  break;
	}
	for (j=sizeof(grps)-1;j>=0;j--) // Erkennung von Gruppen
	  if ((x=tmp[grps[j]]) && intp(j))
	    recval+=x;
	if (pointerp(tmp=tmp[MATERIAL_SYMMETRIC_RECOGNIZABILITY])) {
	  for (j=sizeof(tmp)-2;j>=0;j-=2) {
	    if (!intp(x=tmp[j+1]))
	      raise_error("materialdb: illegal sym.recoc. format\n");
	    if (props[tmp[j]])
	      recval+=x;
	    else // bei passenden Gruppen +, bei anderen -
	      recval-=x;
	  }
	}
      }
    }

    // Jetzt wird ermittelt, ob vielleicht eine ungenauere
    // Beschreibung gegeben werden soll:
    x=dif[0];
    for (i=sizeof(dif)-1;i>=1;i-=2)
      if (recval>=dif[i-1])
	x=dif[i];
    if (x!=mat)
      return MaterialName(x, casus, 100);
  }
  
  if (!pointerp(names=props[P_NAME]) || sizeof(names)<4)
    names=({"unbekanntes Material", "unbekannten Materials",
            "unbekanntem Material", "unbekannten Material"});
  if (casus<0 || casus>3)
    casus=0;
  return names[casus];
}

varargs string ConvMaterialList(mixed mats, int casus, mixed idinf) {
  string *ms,ml;
  int i;
  
  ml="";
  if (mappingp(mats))
    ms=m_indices(mats);
  else if (stringp(mats))
    ms=({mats});
  else if (pointerp(mats))
    ms=mats;
  else
    ms=({});
  for (i=sizeof(ms)-1;i>=0;i--) {
    ml+=MaterialName(ms[i],casus,idinf);
    if (i)
      ml+=((i>1)?", ":" und ");
  }
  return ml;
}

int MaterialGroup(mapping mats, string grp) {
  string *ms,mat;
  int i,res;
  mapping props;

  res=0;
  if (!mappingp(mats) || !stringp(grp))
    return res;
  ms=m_indices(mats);
  for (i=sizeof(ms)-1;i>=0;i--) {
    mat=ms[i];
    if (mappingp(props=materials[mat]))
      res+=(mats[mat]*props[grp])/100;
  }
  if (res<-100) // Vielleicht noch Antimaterie zulassen
    res=-100;   // (noch nicht sicher ob das so bleiben wird oder 0 sein wird)
  if (res>100)
    res=100;
  return res;
}

#define DUMPFILE "/p/daemon/save/MATERIALS"

void Dump() {
  string *mats, *names, *grps, grp,mat;
  int i,j;
  mapping props;

  seteuid(getuid(this_object()));
  rm(DUMPFILE);
  mats=sort_array(m_indices(materials),#'<);
  for (i=sizeof(mats)-1;i>=0;i--) {
    mat=mats[i];
    if (!mappingp(props=materials[mat])
	|| !pointerp(names=props[P_NAME])
	|| sizeof(names)<4)
      continue;
    grps=sort_array(m_indices(props)-EXCLUDE_ARRAY,#'>);
    write_file(DUMPFILE,
	       break_string(sprintf("%s (%s): [%s]\n",
				    mat, names[0], implode(grps,", ")),
			    78,"     ")[5..]);
  }

  write_file(DUMPFILE,"\n -----\n\n");
  
  grps=sort_array(m_indices(material_groups),#'<);
  for (i=sizeof(grps)-1;i>=0;i--) {
    grp=grps[i];
    mats=sort_array(material_groups[grp],#'>);
    write_file(DUMPFILE,
	       break_string(sprintf("%s (%s): [%s]\n",
				    grp, GroupName(grp), implode(mats,", ")),
			    78,"   ")[3..]);
  }
}

string *AllMaterials() {
  return m_indices(materials);
}

string *AllGroups() {
  return m_indices(material_groups);
}

string *GetMatMembership(string mat) {
  mapping props;

  if (!mappingp(props=materials[mat]))
    return ({});
  return m_indices(props)-EXCLUDE_ARRAY;
}

string *GetGroupMembers(string grp) {
  string *mats;

  if (!pointerp(mats=material_groups[grp]))
    return ({});
  return mats;
}

string GroupName(string grp) {
  return ((([MATGROUP_SOLID:"feste Materialien",
	     MATGROUP_FLUID:"Flüssigkeiten",
	     MATGROUP_GAS:"Gase",

	     MATGROUP_WOOD:"Hölzer",
	     MATGROUP_DECIDUOUS_WOOD:"Laubhölzer",
	     MATGROUP_CONIFER_WOOD:"Nadelhölzer",
	     MATGROUP_TROPICAL_WOOD:"Tropenhölzer",
	     MATGROUP_STONE:"Steine",
	     MATGROUP_MINERAL:"Mineralien",
	     MATGROUP_JEWEL:"Edelsteine",
	     MATGROUP_METAL:"Metalle",
	     MATGROUP_PRECIOUS_METAL:"Edelmetalle",
	     MATGROUP_PAPER:"Papierähnliches",
	     MATGROUP_CLOTH:"Stoffe",

	     MATGROUP_ELEMENTAL:"(altertümliche) Elemente",
	     MATGROUP_BIO:"biologische Materialien",
	     MATGROUP_LIVING:"Lebewesen",
	     MATGROUP_HERBAL:"pflanzliche Materialien",
	     MATGROUP_DEAD:"Überreste von Lebewesen",
	     MATGROUP_DRUG:"Drogen und Heiltränke",
	     
	     MATGROUP_SWIMABLE:"schwimmbare Materialien",
	     MATGROUP_INFLAMMABLE:"entflammbare Materialien",
	     MATGROUP_EXPLOSIVE:"Sprengstoffe",
	     MATGROUP_POISONOUS:"Gifte",
	     MATGROUP_ELECTRICAL:"geladene Materialien",
	     MATGROUP_FLEXIBLE:"biegsame Materialien",
	     MATGROUP_MAGNETIC:"anziehende Materialien",
	     MATGROUP_INVIS:"unsichtbare Materialien",
	     MATGROUP_ACIDIC:"Säuren",
	     MATGROUP_EATABLE:"Essbares",
	     MATGROUP_MAGIC:"Magisches",
	     MATGROUP_RADIOACTIVE:"Unbeschreibliches",
	     MATGROUP_HOLY:"heilige Materialien",
	     MATGROUP_UNHOLY:"dämonische Materialien"
	     ])[grp])||"Unbekanntes");
}
