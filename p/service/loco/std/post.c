// SilberLand MUDlib
//
// /p/service/std/post.c -- standard post office
//
// $Date: 1997/03/16 19:11:35 $
/* $Revision: 1.1 $
 * $Log: post.c,v $
 * Revision 1.1  1997/03/16 19:11:35  Woody
 * prepared for revision control
 *
 */

// urspruenglich (C) 1993 by Loco


inherit "/std/room";

#include "/mail/post.h"
#include <properties.h>
#include <moving.h>
#include <rooms.h>

void create() 
{
  int i;
  ::create();
  SetProp(P_SHORT, "Postamt");
  SetProp(P_INT_SHORT,"Postamt");
  SetProp(P_INT_LONG,"\
Dies ist ein Testpostamt.\n\
Von hier aus kannst Du Briefe an Deine Mitspieler schicken und Briefe von\n\
ihnen lesen. Wenn Du das willst, tippe 'post' oder 'mail',\n\
bzw. 'mail <spieler>'.\n");
  AddDetail("postamt","Ein normales Postamt.");  
  SetProp( P_LIGHT, 1 );
  AddCmd("post","do_mail");
  AddCmd("mail","do_mail");
  SetProp(P_INDOORS,1);
//  AddItem("/p/service/loco/std/mailcabin", REFRESH_REMOVE);
//  AddItem("/p/service/loco/std/mailcabin", REFRESH_REMOVE);

}

int do_mail(string str) {
  object mailer;
  if (this_interactive()!=this_player()) return 0;
  mailer=clone_object(MAILER);
//  mailer->SetOfficeName(short());
  mailer->SetOfficeName("Postamt");
//  mailer->do_mail(str);
  mailer->do_mail( ((!str)||(str=="mail")) ? 0 : this_player()->_unparsed_args() );
  return 1;
}

