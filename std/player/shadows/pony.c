#include <defines.h>
#include <moving.h>
#include <properties.h>
#include <language.h>

// 
// Da der Shadow sich nicht im Player befindet, wird ein
// Zusatzobjekt benoetigt.
//
#define HOOK "obj/pony_hook"

static object pl;

create()
{
	if( IS_BLUE(ME) ) return;
  shadow( PL, 1);
	pl=PL;
  clone_object( HOOK ); // Steuerung des shadows ueber die hook
}

_query_title()
{ 
	switch( pl->QueryProp(P_GENDER) )
	{
		case FEMALE: return "die Ponyreiterin";
		default:
			return "der Ponyreiter";
	}
}

_query_msgin() { return "reitet herein"; }
_query_msgout() { return "reitet"; }

move( dest, method, dir, msgout, msgin )
{
	if( !(method & M_NOCHECK) && dest->QueryProp(P_INDOORS) )
	{
		write( "Das Pony weigert sich, dorthin zu traben.\n" );
		return 1;
	}
	return pl->move( dest, method, dir, msgout, msgin );
}

_inventory()
{
	if( !environment() || set_light(0)>0 )
		write( "Du sitzt auf einem Pony.\n" );
	return pl->_inventory();
}

absteigen() { destruct(ME); }

QueryReiter() { return 1; }
