/*
 * Copyright (c) 2006 Steffen Wendzel <steffen (at) wendzel (dot) de>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define __FAVOR_BSD
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>
#include <stdarg.h>
#include <signal.h>

#include "openportd.h"

void		sig_handler(int);
void		read_config(void);
__dead void	usage(void);
int             main(int, char *[]);

int		   daemon_mode;
size_t		   elements;
struct conf_entry *key_entry;
char		  *config_file;

/* reload config file */

void
sig_handler(int id)
{
	/* free old config */
	free(key_entry);
	elements = 0;
	
	syslog(LOG_DAEMON|LOG_NOTICE, "restarting\n");
	read_config();
}

void
read_config()
{
	FILE		*fp;
	char		 line[KEY_SIZ + ACTION_SIZ + 3] = { '\0' };
	char		*ptr;
	unsigned int	 i;

	fp = fopen(config_file, "r");
	if (!fp)
		err(1, config_file);
	
	/* each line comes in the form
	 * "key 'action'", e.g.:
	 * restart-ssh '439t0j34t044zhfg4 pkill -HUP sshd'
	 */
	elements = 0;
	key_entry = NULL;

	while (fgets(line, sizeof(line), fp)) {
		if (line[0] == '\n')
			continue;

		key_entry = (struct conf_entry *) realloc(key_entry,
				sizeof(struct conf_entry) * (elements+1));
		if (!key_entry)
			err(1, "realloc");
		elements ++;
	
		/* find the end of the key */
		for (ptr = line, i = 0; i < KEY_SIZ && ptr != NULL; ptr++, i++)
			if (ptr[0] == ' ' || ptr[0] == '\t') {
				if (i < 8) {
					fprintf(stderr, "a key must at "
						"least contain 8 chars!\n");
				}
				strncpy((key_entry+elements-1)->key, line, i);
				ptr = NULL;
				break; /* break */
			}
		if (ptr != NULL) {
			fprintf(stderr, "parse error in config file\n");
			exit(1);
		}
		/* find the action */
		for (ptr = line + i; i < sizeof(line); ptr++, i++)
			if (ptr[0] != ' ' && ptr[0] != '\t')
				i = sizeof(line); /* break */
		/* save the action value */
		strncpy((key_entry+elements - 1)->action, ptr - 1,
			strlen(ptr));
	}
	
	if (elements < 1) {
		fprintf(stderr, "no keys/actions defined!\n");
		exit(1);
	}
	
	fclose(fp);
}

__dead void
usage(void)
{
	extern char    *__progname;

	fprintf(stderr, "usage: %s [-dh] [-m master-ip] [-f conf-file]\n",
		__progname);
	exit(1);
}

int
main(int argc, char *argv[])
{
	int              ch;
	char		 buf[8192] = {'\0'};
	socklen_t	 size;
	int		 len;
	unsigned int	 max_len;
	unsigned int	 i;
	int		 sockfd;
	char		*content;
	struct ip	*iphdr;
	struct icmp	*icmphdr;
	struct sockaddr_in from;
	char		*master_ip;

	config_file = NULL;
	master_ip = NULL;
	
	while ((ch = getopt(argc, argv, "dhm:f:")) != -1) {
		switch (ch) {
		case 'd':
			daemon_mode = 1;
			break;
		case 'm':
			master_ip = (char *) calloc(1, strlen(optarg) + 1);
			if (!master_ip)
				err(1, "calloc");
			strncpy(master_ip, optarg, strlen(optarg));
			break;
		case 'f':
			config_file = (char *) calloc(1, strlen(optarg) + 1);
			if (!config_file)
				err(1, "calloc");
			strncpy(config_file, optarg, strlen(optarg)); 
			break;
		case 'h':
		default:
			usage();
			/* NOTREACHED */
		}
	}
	
	if (!config_file) {
		config_file = (char *) calloc(1, strlen(CONFIG_FILE) + 1);
		if (!config_file)
			err(1, "calloc");
		strncpy(config_file, CONFIG_FILE, strlen(CONFIG_FILE));
	}
	
	printf("starting openportd...\n");
	
	/* reload config on (p)kill -HUP */
	signal(SIGHUP, sig_handler);
	
	read_config();
	
	bzero(&from, sizeof(struct sockaddr_in));
	size = sizeof(struct sockaddr_in);
	
	if (master_ip)
		if (!inet_pton(AF_INET, master_ip,
		(struct sockaddr_in *) &from.sin_addr))
			err(1, master_ip);
	from.sin_family = AF_INET;
	
	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		err(1, "socket");

	iphdr = (struct ip *) buf;
	icmphdr = (struct icmp *) buf + SIZE_IP;
	
	if (daemon_mode) {
		pid_t pid, sid;
		pid = fork();
		if (pid < 0)
			err(1, "fork");
		if (pid > 0)
			exit(0);
		
		/* child code */
		umask(0);
		sid = setsid();
		if (sid < 0)
			err(1, "setsid");
		
		if ((chdir("/")) < 0)
			err(1, "chdir()");
	}
	
	while ((len = recvfrom(sockfd, buf, sizeof(buf)-1, 0,
	(struct sockaddr *) &from, &size)) >= 0) {
		/* check for a minimum len */
		if (len < SIZE_IP + SIZE_ICMP + 3) {
			continue;
		}
		
		/* discard all pkts with IP options set */
		if (iphdr->ip_hl > 5)
			continue;
		
		/* only accept icmp echo requests, type zero */
		if (icmphdr->icmp_type != 0 && icmphdr->icmp_code != 0)
			continue;
		
		content = (buf + SIZE_IP + SIZE_ICMP);
		
		/* maximum len of bytes to compare in this pkt */
		max_len = (iphdr->ip_len - SIZE_IP - SIZE_ICMP - 1);
		
		for (i = 0; i < elements; i++) {
			if (strlen(key_entry[i].key) < max_len)
				len = strlen(key_entry[i].key);
			else
				len = max_len;
				
			if (strncmp(content, key_entry[i].key, len) == 0) {
				/* found a magic string */
				system(key_entry[i].action);
				syslog(LOG_DAEMON|LOG_NOTICE, "executing %s\n",
					key_entry[i].action);
			}
		}
		bzero(buf, sizeof(buf));
	}
	
	/* NOTREACHED */
	return (0);
}

