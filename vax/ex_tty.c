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
/* Copyright (c) 1981 Regents of the University of California */
static char *sccsid = "@(#)ex_tty.c	1.3	(9.2	2/19/83)";
#include "ex.h"
#include "ex_tty.h"

static char allocspace[256];
static char *freespace;

/*
 * Terminal type initialization routines,
 * and calculation of flags at entry or after
 * a shell escape which may change them.
 */
static short GT;

gettmode()
{

	GT = 1;
#ifndef USG
	if (gtty(2, &tty) < 0)
		return;
	if (ospeed != tty.sg_ospeed)
		value(SLOWOPEN) = tty.sg_ospeed < B1200;
	ospeed = tty.sg_ospeed;
	normf = tty.sg_flags;
	UPPERCASE = (tty.sg_flags & LCASE) != 0;
	if ((tty.sg_flags & XTABS) == XTABS || teleray_glitch)
		GT = 0;
	NONL = (tty.sg_flags & CRMOD) == 0;
#else
	if (ioctl(2, TCGETA, &tty) < 0)
		return;
	if (ospeed != (tty.c_cflag & CBAUD))	/* mjm */
		value(SLOWOPEN) = (tty.c_cflag & CBAUD) < B1200;
	ospeed = tty.c_cflag & CBAUD;
	normf = tty;
	UPPERCASE = (tty.c_iflag & IUCLC) != 0;
	if ((tty.c_oflag & TABDLY) == TAB3 || teleray_glitch)
		GT = 0;
	NONL = (tty.c_oflag & ONLCR) == 0;
#endif
}

setterm(type)
	char *type;
{
	char *tparm();
	register int unknown, i;
	register int l;
	int errret;
	extern char termtype[];

	unknown = 0;
	if (cur_term && exit_ca_mode)
		putpad(exit_ca_mode);
	cur_term = 0;
	strcpy(termtype, type);
	setupterm(type, 2, &errret);
	resetterm();
	if (errret != 1) {
		unknown++;
		cur_term = 0;
		setupterm("unknown", 1, &errret);
		resetterm();
	}
#ifdef TRACE
	if (trace) fprintf(trace, "after setupterm, lines %d, columns %d, clear_screen '%s', cursor_address '%s'\n", lines, columns, clear_screen, cursor_address);
#endif
	i = lines;
	if (lines <= 1)
		lines = 24;
	if (lines > TUBELINES)
		lines = TUBELINES;
	l = lines;
	if (ospeed < B1200)
		l = 9;	/* including the message line at the bottom */
	else if (ospeed < B2400)
		l = 17;
	if (l > lines)
		l = lines;
	/*
	 * Initialize keypad arrow keys.
	 */
	freespace = allocspace;

	kpadd(arrows, key_ic, "i", "inschar");
	kpadd(immacs, key_ic, "\033", "inschar");
	kpadd(arrows, key_eic, "i", "inschar");
	kpadd(immacs, key_eic, "\033", "inschar");

	kpboth(arrows, immacs, key_up, "k", "up");
	kpboth(arrows, immacs, key_down, "j", "down");
	kpboth(arrows, immacs, key_left, "h", "left");
	kpboth(arrows, immacs, key_right, "l", "right");
	kpboth(arrows, immacs, key_home, "H", "home");
	kpboth(arrows, immacs, key_il, "o\033", "insline");
	kpboth(arrows, immacs, key_dl, "dd", "delline");
	kpboth(arrows, immacs, key_clear, "\014", "clear");
	kpboth(arrows, immacs, key_eol, "d$", "clreol");
	kpboth(arrows, immacs, key_sf, "\005", "scrollf");
	kpboth(arrows, immacs, key_dc, "x", "delchar");
	kpboth(arrows, immacs, key_npage, "\006", "npage");
	kpboth(arrows, immacs, key_ppage, "\002", "ppage");
	kpboth(arrows, immacs, key_sr, "\031", "sr");
	kpboth(arrows, immacs, key_eos, "dG", "clreos");

	/*
	 * Handle funny termcap capabilities
	 */
	if (change_scroll_region && save_cursor && restore_cursor) insert_line=delete_line="";
	if (parm_insert_line && insert_line==NULL) insert_line="";
	if (parm_delete_line && delete_line==NULL) delete_line="";
	if (insert_character && enter_insert_mode==NULL) enter_insert_mode="";
	if (insert_character && exit_insert_mode==NULL) exit_insert_mode="";
	if (GT == 0)
		tab = back_tab = NOSTR;

#ifdef SIGTSTP
	/*
	 * Now map users susp char to ^Z, being careful that the susp
	 * overrides any arrow key, but only for hackers (=new tty driver).
	 */
	{
		static char sc[2];
		int i, fnd;

		ioctl(2, TIOCGETD, &ldisc);
		if (!value(NOVICE)) {
			sc[0] = olttyc.t_suspc;
			sc[1] = 0;
			if (olttyc.t_suspc == CTRL(z)) {
				for (i=0; i<=4; i++)
					if (arrows[i].cap[0] == CTRL(z))
						addmac(sc, NULL, NULL, arrows);
			} else
				addmac(sc, "\32", "susp", arrows);
		}
	}
#endif

	value(WINDOW) = options[WINDOW].odefault = l - 1;
	if (defwind)
		value(WINDOW) = defwind;
	value(SCROLL) = options[SCROLL].odefault =
		hard_copy ? 11 : (value(WINDOW) / 2);
	if (columns <= 4)
		columns = 1000;
	if (tparm(cursor_address, 2, 2)[0] == 'O')	/* OOPS */
		cursor_address = 0;
	else
		costCM = cost(tparm(cursor_address, 10, 8));
	costSR = cost(scroll_reverse);
	costAL = cost(insert_line);
	costDP = cost(tparm(parm_down_cursor, 10));
	costLP = cost(tparm(parm_left_cursor, 10));
	costRP = cost(tparm(parm_right_cursor, 10));
	costCE = cost(clr_eol);
	costCD = cost(clr_eos);
	if (i <= 0)
		lines = 2;
	/* proper strings to change tty type */
	termreset();
	gettmode();
	value(REDRAW) = insert_line && delete_line;
	value(OPTIMIZE) = !cursor_address && !tab;
	if (ospeed == B1200 && !value(REDRAW))
		value(SLOWOPEN) = 1;	/* see also gettmode above */
	if (unknown)
		serror("%s: Unknown terminal type", type);
}

/*
 * Map both map1 and map2 as below.  map2 surrounded by esc and i.
 */
kpboth(map1, map2, key, mapto, desc)
struct maps *map1, *map2;
char *key, *mapto, *desc;
{
	char surmapto[30];
	char *p;

	if (key == 0)
		return;
	kpadd(map1, key, mapto, desc);
	if (any(*key, "\b\n "))
		return;
	strcpy(surmapto, "\33");
	strcat(surmapto, mapto);
	strcat(surmapto, "a");
	p = freespace;
	strcpy(p, surmapto);
	freespace += strlen(surmapto) + 1;
	kpadd(map2, key, p, desc);
}

/*
 * Define a macro.  mapstr is the structure (mode) in which it applies.
 * key is the input sequence, mapto what it turns into, and desc is a
 * human-readable description of what's going on.
 */
kpadd(mapstr, key, mapto, desc)
struct maps *mapstr;
char *key, *mapto, *desc;
{
	int i;

	for (i=0; i<MAXNOMACS; i++)
		if (mapstr[i].cap == 0)
			break;
	if (key == 0 || i >= MAXNOMACS)
		return;
	mapstr[i].cap = key;
	mapstr[i].mapto = mapto;
	mapstr[i].descr = desc;
}

char *
fkey(i)
	int i;
{
	if (i < 0 || i > 9)
		return NOSTR;
	switch (i) {
	case 0: return key_f0;
	case 1: return key_f1;
	case 2: return key_f2;
	case 3: return key_f3;
	case 4: return key_f4;
	case 5: return key_f5;
	case 6: return key_f6;
	case 7: return key_f7;
	case 8: return key_f8;
	case 9: return key_f9;
	case 10: return key_f0;
	}
}

/*
 * cost figures out how much (in characters) it costs to send the string
 * str to the terminal.  It takes into account padding information, as
 * much as it can, for a typical case.  (Right now the typical case assumes
 * the number of lines affected is the size of the screen, since this is
 * mainly used to decide if insert_line or scroll_reverse is better, and this always happens
 * at the top of the screen.  We assume cursor motion (cursor_address) has little
 * padding, if any, required, so that case, which is really more important
 * than insert_line vs scroll_reverse, won't be really affected.)
 */
static int costnum;
cost(str)
char *str;
{
	int countnum();

	if (str == NULL || *str=='O')	/* OOPS */
		return 10000;	/* infinity */
	costnum = 0;
	tputs(str, lines, countnum);
	return costnum;
}

/* ARGSUSED */
countnum(ch)
char ch;
{
	costnum++;
}
