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

#define CONFIG_FILE	"/etc/openportd.conf"

#define SIZE_IP		20 /* sizeof IP hdr */
#define SIZE_ICMP	 8 /* sizeof ICMP hdr */

#define KEY_SIZ		0xff
#define ACTION_SIZ	0xfff
#define KEY_LINE	"%254s %254s %4095s"

struct conf_entry {
	char	key[KEY_SIZ];
	char	action[ACTION_SIZ];
};

