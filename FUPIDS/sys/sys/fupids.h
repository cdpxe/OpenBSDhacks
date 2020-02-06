/* fuzzy-user-profile-intrusion-detection-system (fupids)
 +
 * (C) 2003 by Steffen Wendzel <steffen (at) wendzel (dot) de>. all rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Steffen Wendzel.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define UPID_COUNTER	1	/* default value to add */
#define UPID_NEWPROG	65	/* a new programm is opened */

#define UPID_NEWLSTSOK	80	/* new listen()--socket started */
#define UPID_PERMMODNIF	80	/* network interface is set in prom. mode */

/* under development: */
#define UPID_NEWIMPPROG	70	/* secure marked programm */
#define UPID_OPNIMPFILE	72	/* secure marked file is opened */

#define UPID_UARSIZE	32
#define UPID_ARSIZE	96

#define UPID_FUZ_MIN	0
#define UPID_FUZ_NOR	1
#define UPID_FUZ_LOW	2
#define UPID_FUZ_MED	3
#define UPID_FUZ_HIG	4
#define UPID_FUZ_XTR	5

#define UPID_ELEMENT	(VTOI(curproc->p_textvp))->i_number % UPID_ARSIZE
#define UPID_MALC	M_TEMP, M_WAITOK

/*
 * Prog--SLIST
 */
struct upid
{
	SLIST_ENTRY(upid) next;
	ino_t inode;		/* binary's inode */
};
SLIST_HEAD(upidh, upid);

/*
 * User--SLIST
 */
struct mupid
{
	SLIST_ENTRY(mupid) next;		/* this list */
	struct upidh *array[UPID_ARSIZE];	/* proc hash list */
	int uid;
	int atklevel;				/* attacker-level */
};
SLIST_HEAD(mupid_h, mupid);

struct mupid_h *arfupid_hp[UPID_UARSIZE];

void upid_newuser(int uid, int flag);
void upid_newproc(struct mupid *mnp, int flag);
void upid_userfind(int uid, int flag);
void upid_procfind(struct upidh *headp, struct mupid *mnp, int flag);
void upid_checkit(struct upid *np, struct mupid *mnp, int flag);
