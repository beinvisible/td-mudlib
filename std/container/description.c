// SilberLand MUDlib
//
// CONTAINER/DESCRIPTION.C -- standard description for containers
//
// $Date: 2008/09/06 23:00:00 $
/* $Revision: 3.7 $
 * $Log: description.c,v $
 *
 * Revision 3.7  2008/09/06 23:00:00  TamTam
 * Bug stringenize() (CAP() verschluckt '.' in ANSI-Strings)
 *
 * Revision 3.6  1997/09/25 15:07:22  Woody
 * inherited material nicht mehr (-> std/thing/description)
 *
 * Revision 3.5  1997/04/17 01:58:39  Woody
 * inherited jetzt auch std/thing/material
 *
 * Revision 3.4  1997/03/13 01:07:03  Woody
 * prepared for revision control
 *
 * Revision 3.3  1997/03/05 01:30:00  Woody
 * einen kleinen Bug in make_invlist() entfernt, der durch
 * Werte von P_WANTS_TO_LEARN > 1 enstanden ist...
 *
 * Revision 3.2  1996/05/25 19:00:00  Woody
 * column display configurable via player property P_COLUMN_MODE
 *
 * Revision 3.1  1996/04/29 14:40:38  Hate
 * Inventory description changed. one or more columns possible.
 *
 * Revision 3.0  1995/11/22 14:31:35  Jof
 * Updated to revision 3.0
 *
 * Revision 2.5  1995/11/22 14:19:50  Jof
 * Fixed RCS header
 *
 * Revision 2.4  1995/08/24 19:20:46  Jof
 * *** empty log message ***
 *
 * Revision 2.3  1995/02/22  13:05:56  Hate
 * long declared as varargs (Rumata)
 * Hate=?
 *
 * Revision 2.2  1994/03/18  12:09:47  Jof
 * Fixed indentation
 *
 * Revision 2.2  1994/03/18  12:09:47  Jof
 * Fixed indentation
 *
 * Revision 2.1  1994/02/24  09:47:42  Kirk
 * *** empty log message ***
 *
 * Revision 2.0  1994/01/13  16:59:55  mud
 * *** empty log message ***
 *
 * Revision 1.2  1994/01/08  13:10:50  mud
 * #pragma strong_types
 *
 */

inherit "/std/thing/description";

#pragma strong_types

#define NEED_PROTOTYPES

#include <container.h>
#include <properties.h>
#include <defines.h>
#include <wizlevels.h>
#include <wizard.h>
#include <vt100.h>
#include <moving.h>
#include <new_skills.h>

void create()
{
  description::create();
  if (!clonep(this_object())) return;
  SetProp(P_TRANSPARENT, 1);
  AddId("Container");
}

varargs string long() {
  string descr, inv_descr;

  descr = process_string(QueryProp(P_LONG));
  if(!QueryProp(P_TRANSPARENT)) return descr;

  inv_descr = make_invlist(PL, all_inventory(ME), 0);
  if ( inv_descr != "" )
    descr += capitalize(QueryPronoun()) + " enthält:\n" + inv_descr;
  return descr;
}

object vwr;

#define FT_NPC  1
#define FT_PLR  2
#define FT_ARM  3
#define FT_WPN  4

string wiz_hdr(mixed obj)
{
  string descr, ter, fnam, nc, sc, tc, cc,
         lvc, mvc, mmvc, hpc, mhpc, spc, mspc, coff, *fn;
  int fs, mlev, ftype;

  if (!vwr && !objectp(vwr))
    return object_name(obj);
  mlev=vwr->QueryProp(P_WANTS_TO_LEARN);
  descr="";
  switch (vwr->QueryProp(P_TTY)) {
    case "vt100":
    case "ansi": coff=VT_TA_AO; hpc=VT_GREEN; sc=VT_TA_B+VT_BLUE;
                 tc=spc=VT_BLUE; mhpc=mspc=mmvc=VT_TA_B+VT_RED; nc=VT_GREEN;
                 cc=mvc=VT_CYAN; lvc=VT_TA_B+VT_GREEN; break;
    default: coff=nc=sc=tc=mvc=mmvc=lvc=spc=mspc=hpc=mhpc=cc="";
  }
  if (obj->id("NPC"))
    ftype=FT_NPC;
  else
    if (obj->id("Spieler"))
      ftype=FT_PLR;
    else
      if (obj->QueryProp(P_WC))
        ftype=FT_WPN;
      else
        if (obj->QueryProp(P_AC))
          ftype=FT_ARM;
  if (mlev&RH_NFNHDR)
    descr+=sprintf("%s%s%s:", tc,
        (ftype==FT_NPC?"NPC":(ftype==FT_PLR?"PLR":
        (ftype==FT_WPN?"Wpn":(ftype==FT_ARM?"Arm":"??")))), coff);
  fnam=object_name(obj);
  fn=explode(fnam, "/");
  descr+=nc;
  if (mlev&RH_NFILENAME && sizeof(fn)) {
    switch (fn[0]) {
      case "players": descr+="~"+fn[1]+"/";
                      if (sizeof(fn)>3)
                        descr+=".../";
                      descr+=fn[<1]; break;
      case "d": descr+="+"+capitalize(fn[1][0..1])+"/"+fn[2]+"/.../"+fn[<1]; break;
      default: descr+="/"+implode(fn, "/");
    }
  }
  else
    descr+=fnam;
  descr+=coff;
  if (mlev&RH_SIZE && ftype!=FT_PLR) {
    fs=file_size(explode(fnam, "#")[0]+".c");
    descr+=sprintf("|sz:%s%s%s", sc, (fs<1024?(string)fs:((string)(fs/1024)+"k")), coff);
  }
  switch(ftype) {
    case FT_NPC: if (mlev&RH_NPC_LVL)
                   descr+=sprintf("|lv:%s%d%s", lvc, obj->QueryProp(P_LEVEL), coff);
                 if (mlev&RH_NPC_HP)
                   descr+=sprintf("|hp:%s%d%s(%s%d%s)", hpc, obj->QueryProp(P_HP),
                          coff, mhpc, obj->QueryProp(P_MAX_HP), coff);
                 if (mlev&RH_NPC_SP)
                   descr+=sprintf("|sp:%s%d%s(%s%d%s)", spc, obj->QueryProp(P_SP),
                          coff, mspc, obj->QueryProp(P_MAX_SP), coff);
                 break;
    case FT_PLR: if (mlev&RH_NPC_LVL)
                   descr+=sprintf("|lv:%s%d%s", lvc, obj->QueryProp(P_LEVEL), coff);
                 if (mlev&RH_NPC_HP)
                   descr+=sprintf("|hp:%s%d%s(%s%d%s)", hpc, obj->QueryProp(P_HP),
                          coff, mhpc, obj->QueryProp(P_MAX_HP), coff);
                 if (mlev&RH_NPC_SP)
                   descr+=sprintf("|sp:%s%d%s(%s%d%s)", spc, obj->QueryProp(P_SP),
                          coff, mspc, obj->QueryProp(P_MAX_SP), coff);
                 if (mlev&RH_PLR_MV)
                   descr+=sprintf("|mv:%s%d%s(%s%d%s)", mvc, obj->QueryProp(P_MV),
                          coff, mmvc, obj->QueryProp(P_MAX_MV), coff);
                 break;
  }
/*  if (mlev&RH_CLONER)
    descr+=sprintf("|cl:%s%s%s", cc, obj->QueryProp(P_CLONER), coff); */

  return descr;
}


// flags: 1 - wizard, 2 - don't collect equal objects '
private void stringenize(mixed obj, int flags, mixed objs, mixed info)
{
  string id, tmp;
  int idx;
  tmp = capitalize(obj->short()||"");
  if(tmp[<1]=='\n')
    tmp=tmp[0..<2]; // Nur '\n' abschneiden, CAP(ANSI-str) verschluckt das
  tmp=tmp + (flags & 1 ? " ["+wiz_hdr(obj)+"]" : "");
  if(flags & 3 || living(obj)) id = object_name(obj);
  else
    id = explode(object_name(obj), "#")[0] + tmp;
  if((idx = member(objs, id)) == -1)
  {
    objs += ({ id });
    info += ({ ({ tmp, 1, obj}) });
  }
  else
    info[idx][1]++;
}

private string collect(mixed obj)
{
  if(!sizeof(obj[0])) return 0;
  return obj[0] + (obj[1] > 1 ? " ("+obj[1]+")" : "");
}

// flags: 1 - return array, 2 - don't collect equal objects '
varargs string make_invlist(object viewer, mixed inv, int flags)
{
  int max, iswiz, col;
  mixed objs, info;
  string descr;

  // !!x, damit iswiz in jedem Fall 0 oder 1 ist und nichts anderes.
  vwr=viewer;
  iswiz = IS_LEARNER( viewer ) && !!viewer->QueryProp(P_WANTS_TO_LEARN);
  descr = ""; objs = ({}); info = ({});
  map(inv, #'stringenize/*'*/, iswiz | (flags & 2), &objs, &info);
  if(flags & 1) return info;
  inv = map(info, #'collect/*'*/) - ({ 0 });
  if(!sizeof(inv)) return "";
#ifdef DEBUG
  if(!viewer)
    if (find_player("woody"))
      tell_object(find_player("woody"), sprintf(
      "*** std/container/description.c::make_invlist() :\n"
      "    no viewer! TP: %O, PO: %O, PO(1): %O\n",
      this_player(), previous_object(), previous_object(1)));
#endif
  if ((col=(viewer?viewer->QueryProp(P_COLUMN_MODE):0)) == 2)
    col = sizeof(inv) > 6;
  return sprintf("%"+(col ? "#" : "=")+"-78s", implode(inv, "\n")) + "\n";
}

