//
// Wunderland Mudlib
//
// secure/sefun/strlen.c -- strlen fuer ldmud 3.5
//
// $Log: strlen.c,v $
// Revision 1.1  2021/03/25 16:07:06  Fuchur
// Update auf ldmud 3.6.3
//

#if ! __EFUN_DEFINED__(strlen)

int strlen(string str)
{
    return sizeof(str);
}

#endif
