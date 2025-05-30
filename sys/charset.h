//
// Wunderland Mudlib
//
// sys/charset.h -- Makros fuer get_connection_charset()
//
// $Log: charset.h,v $
// Revision 1.1  2021/03/25 16:07:06  Fuchur
// Update auf ldmud 3.6.3
//

#ifndef LPC_CHARSET_H_
#define LPC_CHARSET_H_

/* Mode values recognized by the efun get_combine_charset()
 * and get_connection_charset() */

#define CHARSET_VECTOR     0  /* Return the charset as bitvector array */
#define CHARSET_STRING     1  /* Return the charset as string */
#define CHARSET_QUOTE_IAC  2  /* Return the status of the IAC quoting */

#endif /* LPC_CHARSET_H_ */
