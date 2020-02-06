.PATH:		${.CURDIR}/..

PROG=	openportd
SRCS=	openportd.c
CFLAGS+= -Wall -I${.CURDIR}
CFLAGS+= -Wstrict-prototypes -Wmissing-prototypes
CFLAGS+= -Wmissing-declarations
CFLAGS+= -Wshadow -Wpointer-arith -Wcast-qual
CFLAGS+= -Wsign-compare
CFLAGS+= -ggdb
YFLAGS=
MAN=	openportd.8 openportd.conf.5

.include <bsd.prog.mk>

aclean :
	rm -f *~

