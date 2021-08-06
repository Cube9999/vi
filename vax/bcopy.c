/*
********************************************************************************
*                         Copyright (c) 1985 AT&T                              *
*                           All Rights Reserved                                *
*                                                                              *
*                                                                              *
*          THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T                 *
*        The copyright notice above does not evidence any actual               *
*        or intended publication of such source code.                          *
********************************************************************************
*/
/* block copy from from to to, count bytes */

static char SCCSid[] = "@(#)bcopy.c	1.4";
bcopy(from, to, count)
#ifdef vax
	char *from, *to;
	int count;
{

	asm("	movc3	12(ap),*4(ap),*8(ap)");
}
#else
# ifdef u3b		/* movblkb only works with register args */
	register char *from, *to;
	register int count;
{
	asm("	movblkb	%r6, %r8, %r7");
}
# else
	register char *from, *to;
	register int count;
{
	while ((count--) > 0)
		*to++ = *from++;
}
# endif
#endif
