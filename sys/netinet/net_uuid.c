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

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");


#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/queue.h>

#include <netinet/net_uuid.h>

// Tag type definitions are private to the implementation
#define TAG_TYPE_UUID_STAMP	(1 | MTAG_PERSISTENT)

static MALLOC_DEFINE(M_NET_UUID_LIST_ENTRY, MTAG_UUID_LIST_ENTRY_MEM_NAME,
    "entry in a list of mbuf uuid tags");


#define TAG_UUID_ALLOC_LEN	(sizeof(struct mtag_uuid) \
	       		       - sizeof(struct m_tag))

static void
net_uuid_generate(struct uuid *uuid)
{
	// Generate a v1 UUID for this packet
	kern_uuidgen(uuid, 1);
}

static void
net_uuid_copy(struct uuid *from, struct uuid *to)
{
	if (to == NULL)
		return;
	if (from != NULL)
		memcpy(to, from, sizeof(struct uuid));
	else
		uuid_generate_nil(to);
}

static inline struct mtag_uuid *
net_uuid_alloc_stamp_tag()
{
	return (struct mtag_uuid *) m_tag_alloc(
			MTAG_COOKIE_NET_UUID,
			TAG_TYPE_UUID_STAMP,
			TAG_UUID_ALLOC_LEN,
			M_NOWAIT);
}

static void
net_uuid_free_stamp_tag(struct m_tag *tag)
{
	// We're going to BFS for all sub-tags to free, so set up a FIFO
	STAILQ_HEAD(freeq_head, freeq_entry) head = 
		STAILQ_HEAD_INITIALIZER(head);

	struct freeq_entry {
		STAILQ_ENTRY(freeq_entry) entries;
		struct mtag_uuid *tag;
	} *entry;

	STAILQ_INIT(&head);

	// Put the input tag in the queue
	entry = malloc(sizeof(struct freeq_entry), 
			M_NET_UUID_LIST_ENTRY, M_NOWAIT);
	STAILQ_INSERT_HEAD(&head, entry, entries);

	while (!STAILQ_EMPTY(&head)) {
		entry = STAILQ_FIRST(&head);
		STAILQ_REMOVE_HEAD(&head, entries);

		// Add any sub-tags to the queue
		struct mtag_uuid *tag = entry->tag;
		//struct m_tag *t = (struct m_tag *)tag;
		struct mtag_uuid *branches[] = 
				{ tag->parent, tag->child, tag->sibling };
		for (uint8_t i=0; i < 3; i++) {
			if (branches[i] != NULL) {
				struct freeq_entry *branch = malloc(
						sizeof(struct freeq_entry),
						M_NET_UUID_LIST_ENTRY,
						M_NOWAIT);
				branch->tag = branches[i];
				STAILQ_INSERT_TAIL(&head, branch, entries);
			}
		}

		// We have to use a function pointer to call
		// m_free_tag_default() because it is inaccessible to us
		tag->m_free_tag_default(&tag->tag);
		free(entry, M_NET_UUID_LIST_ENTRY);
	}
}

static struct mtag_uuid *
net_uuid_construct_stamp_tag()
{
	struct mtag_uuid *tag = net_uuid_alloc_stamp_tag();
	tag->parent  = NULL;
	tag->child   = NULL;
	tag->sibling = NULL;
	tag->m_free_tag_default = tag->tag.m_tag_free;
	tag->tag.m_tag_free = &net_uuid_free_stamp_tag;
	return tag;
}

static struct mtag_uuid *
net_uuid_tag_deep_copy(struct mtag_uuid *tag)
{
	struct mtag_uuid *copy = net_uuid_construct_stamp_tag();
	net_uuid_copy(&tag->uuid, &copy->uuid);
	if (tag->parent != NULL)
		copy->parent = net_uuid_tag_deep_copy(tag->parent);
	if (tag->child != NULL)
		copy->child = net_uuid_tag_deep_copy(tag->child);
	if (tag->sibling != NULL)
		copy->sibling = net_uuid_tag_deep_copy(tag->sibling);
	return copy;
}

void
net_uuid_tag_packet(struct mbuf *packet)
{
	struct mtag_uuid *tag;

	tag = net_uuid_construct_stamp_tag();
	net_uuid_generate(&tag->uuid);
	m_tag_prepend(packet, &tag->tag);
}

void
net_uuid_tag_child_packet(struct mbuf *parent, struct mbuf *child)
{
	struct mtag_uuid *parent_tag, *child_tag;

	// Find the uuid tag of the parent
	parent_tag = (struct mtag_uuid *)m_tag_locate(
			parent,
			MTAG_COOKIE_NET_UUID,
			TAG_TYPE_UUID_STAMP,
			NULL);

	// ip_fragment turns the parent packet into the first child,
	// so we cannot assume that child is a fresh mbuf with no uuid tag
	child_tag = (struct mtag_uuid *)m_tag_locate(
			child,
			MTAG_COOKIE_NET_UUID,
			TAG_TYPE_UUID_STAMP,
			NULL);
	if (child_tag != NULL) {
		m_tag_unlink(child, &child_tag->tag);
		if (child_tag != parent_tag) {
			m_tag_free(&child_tag->tag);
		}
	}

	// Allocate new child tag
	child_tag = net_uuid_construct_stamp_tag();
	net_uuid_generate(&child_tag->uuid);
	m_tag_prepend(child, &child_tag->tag);

	child_tag->parent = net_uuid_tag_deep_copy(parent_tag);
}
