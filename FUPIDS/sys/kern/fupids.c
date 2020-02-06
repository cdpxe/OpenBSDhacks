/* fupids code v. 0.0.4
 * Copyright (c) 2003 Steffen Wendzel. All rights reserved.
 * <steffen (at) wendzel (dot) de>
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
 * 4. Neither the name of the author nor the names of its contributors
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

/* Changelog:	0.0.4	- sys_listen now prints out the programm name too
 *			- first patch available
 *		0.0.3	- prom. mode evaluation + logging
 *			- listen()-syscall evaluation
 *		0.0.2	- removed floating point calculations
 *			- new calc system (it is also a litle bit better for new
 *			  users who produces a lot of warnings in the last version)
 *			- removed counter for the programms, it was not really
 *			  sinfull and takes a litle bit of cpu-time.
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/proc.h>
#include <sys/vnode.h>
#include <sys/malloc.h>
#include <ufs/ufs/quota.h>
#include <ufs/ufs/inode.h> /* need VTOI */

#include <sys/fupids.h>

void
upid_newuser(int uid, int flag)
{
	struct mupid_h *ulisth;
	struct mupid *new = (struct mupid *)malloc(sizeof(struct mupid), UPID_MALC);

	/* init values */
	new->uid = curproc->p_cred->p_ruid;
	new->atklevel = 1;

	/* setup an proc--hash for the new user */
	bzero(new->array, UPID_ARSIZE*sizeof(struct upidh));
	
	if(arfupid_hp[uid % UPID_UARSIZE] == NULL) {
		ulisth = (struct mupid_h *)malloc(sizeof(struct mupid_h), UPID_MALC);
		/* main array element to new listheader */
		arfupid_hp[uid % UPID_UARSIZE] = ulisth;
		SLIST_NEXT(new, next) = NULL;
		SLIST_INSERT_HEAD(ulisth, new, next);
	} else {
		printf("just a notice for debugging: new user in existing list\n");
		SLIST_NEXT(new, next) = NULL;
		SLIST_INSERT_HEAD(arfupid_hp[uid % UPID_UARSIZE], new, next);
	}

	if(curproc->p_textvp->v_type == VT_UFS){
		upid_newproc(new, flag);
	}
}

void
upid_newproc(struct mupid *mnp, int flag)
{ 
	struct inode *in;
	struct upidh *plisth;
	struct upid *new = (struct upid *)malloc(sizeof(struct upid), UPID_MALC);

	in = VTOI(curproc->p_textvp);
	new->inode = in->i_number;

	if(mnp->array[in->i_number % UPID_ARSIZE] == NULL) {
		/* allocate memory for the new proglist header */
		plisth = (struct upidh *)malloc(sizeof(struct upidh), UPID_MALC);

		/* now set the slist--header addr to the array element */
		mnp->array[in->i_number % UPID_ARSIZE] = plisth;

		/* set the next element of the programm struct to NULL
		 * (this is our first)
		 */
		SLIST_NEXT(new, next) = NULL;
		SLIST_INSERT_HEAD(plisth, new, next);
	} else { /* just create a new prog--slist member */
		SLIST_NEXT(new, next) = NULL;
		SLIST_INSERT_HEAD(mnp->array[in->i_number % UPID_ARSIZE], new, next);
	}

	printf("fupids: new programm: '%s', uid: %i\n", curproc->p_comm, mnp->uid);
	upid_checkit(new, mnp, UPID_NEWPROG+flag);
}

void
upid_userfind(int uid, int flag)
{
	struct mupid *np;

	if(arfupid_hp[uid % UPID_UARSIZE] != NULL) {
		SLIST_FOREACH(np, arfupid_hp[uid % UPID_UARSIZE], next){
			if(np->uid == uid){
				upid_procfind(np->array[UPID_ELEMENT], np, flag);
				return;
			}
		}
	}
	/* not found */
	printf("fupids: new user, uid %i.\n", uid);
	upid_newuser(uid, flag);
}

void
upid_procfind(struct upidh *headp, struct mupid *mnp, int flag)
{
	struct upid *np;
	struct inode *in;

	if(curproc->p_textvp->v_type == VT_UFS){
		/* search the programm list */
		if(headp != NULL) {
			SLIST_FOREACH(np, headp, next){
				in = VTOI(curproc->p_textvp);
				if(np->inode == in->i_number) {
					upid_checkit(np, mnp, UPID_COUNTER+flag);
					return;
				}
			}
		}
		/* hmm.. not found! */
		upid_newproc(mnp, flag);
	}
}

/* this is fupids brain
 */

void
upid_checkit(struct upid *np, struct mupid *mnp, int flag)
{
	/* filter level configuration */
	int fuzzy[][2] = {	/* [0] is the level */
		{ 300,  5 },	/* [1] is how much i remove on every call */
		{ 400,  7 },
		{ 700, 30 },	/* low level */
		{ 800, 50 },	/* medium */
		{ 900, 80 },	/* high */
		{ 999, 99 }	/* not a real-level, more a filter for log-entrys */
	};
	
	/* calc. the new attacker-level */
	mnp->atklevel += flag;
	
	/* now the simple fuzzy code (works fine) */
	if(mnp->atklevel >= fuzzy[UPID_FUZ_XTR][0]) {
		printf("fupids: max. attack level from uid %d!\n", mnp->uid);
		mnp->atklevel -= fuzzy[UPID_FUZ_XTR][1];
	} else if (mnp->atklevel >= fuzzy[UPID_FUZ_HIG][0]) {
		printf("fupids: high security warning, uid %d, %d!\n", mnp->uid, mnp->atklevel);
		mnp->atklevel -= fuzzy[UPID_FUZ_HIG][1];
	} else if (mnp->atklevel >= fuzzy[UPID_FUZ_MED][0]) {
		printf("fupids: medium security warning, uid %d,%i!\n", mnp->uid, mnp->atklevel);
		mnp->atklevel -= fuzzy[UPID_FUZ_MED][1];
	} else if (mnp->atklevel >= fuzzy[UPID_FUZ_LOW][0]) {
		printf("fupids: low security warning, uid %d,%i!\n", mnp->uid, mnp->atklevel);
		mnp->atklevel -= fuzzy[UPID_FUZ_LOW][1];
	} else if (mnp->atklevel >= fuzzy[UPID_FUZ_NOR][0]) {
		mnp->atklevel -= fuzzy[UPID_FUZ_NOR][1];
	} else if (mnp->atklevel >= fuzzy[UPID_FUZ_MIN][0]) {
		mnp->atklevel -= fuzzy[UPID_FUZ_MIN][1];
	}
}
