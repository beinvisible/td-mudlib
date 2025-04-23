// Tamedhon MUDlib
//
// pkmaster.h
//
// Serii@Tamedhon

#ifndef __PKMASTER_H__
#define __PKMASTER_H__

#define SAVE_FILE "/p/daemon/pkmaster/pkm"
#define PKM "/p/daemon/pkmaster/pkm"
#define PKMASTER "/p/daemon/pkmaster"
#define PK_INFO "\ngpkinfos"
#define PK_COUNT "pkcount"
#define PK_LAST "pklast"
#define PK_TIME "pktime"
#define PK_RACE "pkrace"
#define PKPI_KNR 0
#define PKPI_VRA 1
#define PKPI_VPS 2
#define PKPI_KPS 3
#define PKPI_KTM 4



#undef DB
#define DB(x) \
   if (find_player("serii")) \
      tell_object(find_player("serii"), sprintf("[db] %O\n",x))

#endif