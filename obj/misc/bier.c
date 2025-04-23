#include <properties.h>
#include <defines.h>
#include <language.h>

inherit "std/thing";

mixed sorten;
mixed badsorten;
mixed alisorten;
mixed wahl;

void ungewaehlt();
void initSorten();

void create()
{
    ::create(); 
    if (!clonep(this_object())) return;
    SetProp( P_GENDER, FEMALE );
    SetProp( P_NAME, "Bierflasche" );
    SetProp( P_WEIGHT, 800 );
    SetProp( P_VALUE, 300 );
    SetProp( P_NOBUY, 1 );
    ungewaehlt(); // <-- setzt P_SHORT und P_IDS
    AddCmd( ({"wuensche","wünsche"}), "wuenschen" );
    AddCmd( "bierliste", "liste" );
    AddCmd( ({"trinke","trink"}), "trinken" );
    AddCmd( ({"proste","prost","prosit"}), "prosten" );
    initSorten();
    wahl = -1;
}

void ungewaehlt()
{
    SetProp( P_SHORT, "Eine magische Bierflasche" );
    SetProp( P_LONG,
     "Diese magische Bierflasche ist momentan leer. Und das ist gut so, denn "
    +"Du solltest Dich erst für Deine Lieblingssorte entscheiden, damit die "
    +"magische Flasche Deinen Wünschen entspricht. \n"
    +"(Eine Speisekarte erhältst du mit \"bierliste\".)"
    );
    SetProp( P_IDS, ({ "bier","flasche","humpen","bierflasche" }) );
}

void initSorten()
{
    sorten = ([
        "krombacher" : "Krombacher Pilsner"; "",
        "becks"      : "Becks"; "",
        "jever"      : "Jever"; "",
        "flensburger": "Flensburger Pilsner"; "",
        "warsteiner" : "Warsteiner"; "",
        "veltins"    : "Veltins"; "",
        "bitburger"  : "Bitburger Pilsner"; "",
        "könig"      : "König Pilnser"; "Du fühlst Dich wie ein König.\n",
        "rolinck"    : "Rolinck Pilsner"; "Es war für Dich allein gebraut.\n",
        "iserlohner" : "Iserlohner Pilsner"; "",
        "paulaner"   : "Paulaner"; "Spülwasser.\n",
        "erdinger"   : "Erdinger"; "Es schmeckt ungewöhnlich eklig.\n",
        "radler"     : "Radler"; "Erfrischend!\n",
        "krefelder"  : "Krefelder"; "",
        "sion"       : "Sion Kölsch"; "Urin aus der Flasche.\n",
        "dom"        : "Dom Kölsch"; "Flaschenurin.\n",
        "pinkus"     : "Pinkus Alt"; "",
        "köstritzer": "Köstritzer Schwarzbier";"Das beste Bier der Welt.\n",
        "radeberger" : "Radeberger Pilsner"; "DAS Standardbier des Ostens.\n",
        "kilkenny"     : "Irisches Red Ale"; "",
        "zwickl"     : "Aktien Zwickl Kellerbier"; "",
        "guiness"     : "Guiness Extra Stout"; "",
    ]);
    alisorten = ([
        "flens" : "flensburger",
        "flepi" : "flensburger",
        "köpi" : "könig",
        "bit"   : "bitburger"
    ]);
    badsorten = ([
        "rolink" : "Du meinst doch Rolinck, oder?\n",
        "clausthaler" : "Die Bierflasche schüttelt sich vor Ekel.\n"
    ]);
}

int liste(string str )
{
    string ans;
    int i;

    write( "Es stehen momentan folgende Sorten zur Verfügung: \n" );
    walk_mapping( sorten, lambda( ({ 'k, 'txt }), ({
        #'printf, "  %s\n", 'txt }) ));
    return 1;
}

int wuenschen(string str )
{
    int i;
    string bad;

    notify_fail( "WAS wünschst Du Dir denn?\n" );
    if( !str ) return 0;
    str = lower_case(str);
    if( stringp(alisorten[str]) )
    { // Aliase sollen auch als id erlaubt sein.
        AddId( str );
        str = alisorten[str];
    }

    if( stringp(sorten[str,0]) )
    {
        wahl = str;
        write("Eine gute Wahl! Und schon füllt sich die Flasche mit frischem "
              + sorten[wahl,0] + ".\n" );
        SetProp( P_SHORT, "Eine Flasche voll " + sorten[wahl,0] );
        SetProp( P_LONG,"Du hältst ein schönes, großes und "
                "vor allem kühles " + sorten[wahl,0] + " in Deinen Händen.\n"
        );
        AddId( wahl );
        return 1;
    }

    if( bad = badsorten[str] )
    {
        write( bad );
        return 1;
    }

    if( member( str, ({ "pilz","bier","pils","alt","koelsch","kölsch",
        "hefe","weizen","weizenbier","altbier" }) ) != -1
    )
    {
        write( "Ein bischen genauer sollte es schon sein.\n" );
        return 1;
    }
    write( "So ein Pech, die Sorte ist gerade aus.\n" );
    log_file( "rumata/biere", ctime(time()) + ": "
        + PL->name(WER) + " möchte ein " + capitalize(str) + ".\n"
    );
    return 1;
}

int trinken(string str )
{
    if( !str || !id(str) ) return 0;
    if( !stringp(wahl) )
    {
        write( "Du nuckelst an einer leeren Bierflasche.\n" );
        say( PL->name(WER,2)
            + " nuckelt an einer leeren Bierflasche herum (So ein Kind!)\n"
        );
        return 1;
    }
    write( "Ahhh, Du trinkst Dein " + sorten[wahl,0]
        + " mit Genuss in einem Zug auf.\n" + sorten[wahl,1]
    );
    if( random(3) == 0 )
    {
        write( "Du hörst den Teufel leise lachen:\n"
              +"  'Nichts geht schneller an die Niere,\n"
              +"   als Don Rumata seine Biere.'\n"
        );
    }
    say( PL->name(WER,2) + " trinkt " + PL->QPP( NEUTER, WEN ) + " "
        + sorten[wahl,0] + ".\n" + capitalize(PL->QueryPronoun(WER))
        + " scheint es sichtlich zu genießen.\n"
    );
    ungewaehlt();
    wahl = -1;
    return 1;
}

int prosten(string str )
{
    string arg;
    object obj;

    if( !stringp(wahl)  )
    {
        write( "Mit einer leeren Flasche?\n" );
        return 1;
    }
    if( !str )
    {
        write( "Du prostest der Gemeinde zu.\n" );
        say( PL->name(WER,2) + " sagt Prost!\n" );
        return 1;
    }
    if( sscanf( str, "%s zu", arg ) == 1 )
        str = arg;
    if( !(obj=present(str,environment(PL))) || !living(obj) )
    {
        write( "Keinen Saufbruder gefunden (So'n Frust).\n" );
        return 1;
    }
    write( "Du prostest " + obj->name(WEM,2) + " zu.\n" );
    tell_object( obj, PL->name(WER,2) + " prostet Dir zu.\n" );
    say( PL->name(WER,2) + " prostest " + obj->name(WEM,2) + " zu.\n", obj );
    return 1;
}
