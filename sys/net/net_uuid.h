/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2017 Peter Lotts
 * All rights reserved.
 *
 * Portions of this software were developed by BAE Systems, the University of
 * Cambridge Computer Laboratory, and Memorial University under DARPA/AFRL
 * contract FA8650-15-C-7558 ("CADETS"), as part of the DARPA Transparent
 * Computing (TC) research program.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef _SYS_NET_UUID_H_
#define _SYS_NET_UUID_H_

// Short name used for malloc on list entries, 
// exposed to userspace to allow tracing
#define	MTAG_UUID_LIST_ENTRY_MEM_NAME	"mtag_uuid_list_entry"

#ifdef _KERNEL

#include <sys/mbuf.h>
#include <sys/uuid.h>

// This is defined here so that DTrace can use it - does it need it?
struct mtag_uuid {
	struct m_tag		 tag;
	struct uuid		 uuid;
	struct mtag_uuid	*parent;
	struct mtag_uuid	*child;
	struct mtag_uuid	*sibling;
	void			(*m_free_tag_default)(struct m_tag *);
};

struct mtag_uuid *
	net_uuid_tag_packet(struct mbuf *);
struct mtag_uuid *
	net_uuid_tag_child_packet(struct mbuf *, struct mbuf *);
void	net_uuid_tag_assembled_packet(struct mbuf *, struct mbuf *);
void	net_uuid_tag_move(struct mbuf *, struct mbuf *);

char *	net_uuid_get_uuid_str(char, void *);
char *	net_uuid_get_uuid_str_mbuf(struct mbuf *);
char *	net_uuid_get_uuid_str_tag(struct mtag_uuid *);
char *	net_uuid_get_uuid_str_uuid(struct uuid *);

#endif /* _KERNEL */
#endif /* _SYS_NET_UUID_H_ */
