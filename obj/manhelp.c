// SilberLand MUDlib
//
// obj/manhelp.c -- flyer for collecting results from intermud man
//
// $Date$
/* $Revision$
 * $Log$
 */

/*
 * uebernommen aus der Wunderland Mudlib
 * Zettel, der die Ergebnisse des intermud-man sammelt
 * Autor: Fiona@Wunderland
 */

inherit "/std/thing";

#include <properties.h>
#include <udp.h>
#pragma strong_types

static string txt;
string liestxt();

void create() {
    if (!clonep(this_object())) return;
    ::create();
    SetProp(P_LONG, "Dies ist ein leerer Zettel.\n");
    SetProp(P_SHORT, "Ein Zettel");
    SetProp(P_NAME, "Zettel");
    SetProp(P_GENDER, MALE);
    AddId("zettel");
    SetProp(P_WEIGHT, 0);
    SetProp(P_VALUE, 0);
    SetProp(P_READ_MSG, #'liestxt);
    txt="";
}

public void SetKey(string key) {
    SetProp(P_LONG, break_string("Auf diesem Zettel sammelt der Internet-"
        "Daemon die Ergebnisse Deiner Manual-Anfrage nach \'"+key+
        "\'. Wenn etwas angekommen ist, kannst Du es hierauf lesen.",78));
}

public void udp_reply(mapping data) {
    if (data[SYSTEM]) {
        if (data[SYSTEM]==TIME_OUT) {
            txt+="Manualanfrage nach "+data[NAME]+" 'timed out'.\n";
        }
    } else {
        txt+=sprintf("%'-'|78s\n%s", " "+data[NAME]+" ", data[DATA]);
    }
}

string liestxt() {
    if (!sizeof(txt)) return "Auf dem Zettel steht noch nichts...\n";
    // dummer trick, weil eingebautes lesen kein more hat...
    this_player()->More(txt);
    return "";
}

varargs int move(mixed dest, int meth) {
    if (environment()) call_out("zerfall",5);
    return ::move(dest, meth);
}

void zerfall() {
    object env;
    env=environment();
    if (env) {
        if (living(env)) tell_object(env, "Der Zettel zerfällt.\n");
        else tell_room(env, "Der Zettel zerfällt.\n");
    }
    remove();
}

/*
int remove() {
    destruct(this_object());
    return 1;
}
*/

