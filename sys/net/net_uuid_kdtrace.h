/*-
 * SPDX-License-Identifier: BSD-2-Clause-FreeBSD
 *
 * Copyright (c) 2018 Peter Lotts
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#ifndef _SYS_NET_UUID_KDTRACE_H_
#define	_SYS_NET_UUID_KDTRACE_H_

#include <net/net_uuid.h>
#include <sys/malloc.h>
#include <sys/queue.h>
#include <sys/param.h>
#include <sys/sdt.h>

#define NET_UUID_PROBE(mod, probe, mbuf)			do {	\
	char *str = net_uuid_get_uuid_str(mbuf);			\
	SDT_PROBE1(net_uuid, mod, , probe, str);			\
	free(str, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE_W_PTR(mod, probe, mbuf)			do {	\
	char *str = net_uuid_get_uuid_str(mbuf);			\
	SDT_PROBE2(net_uuid, mod, , probe, str, mbuf);			\
	free(str, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE2(mod, probe, mbuf, arg1)			do {	\
	char *str = net_uuid_get_uuid_str(mbuf);			\
	SDT_PROBE2(net_uuid, mod, , probe, str, arg1);			\
	free(str, M_TEMP);						\
} while (0)

SDT_PROVIDER_DECLARE(net_uuid);

SDT_PROBE_DECLARE(net_uuid, mem, ,	alloc);

SDT_PROBE_DECLARE(net_uuid, packet, ,	trace__start);
SDT_PROBE_DECLARE(net_uuid, packet, ,	trace__stop);
SDT_PROBE_DECLARE(net_uuid, packet, ,	fragment);
SDT_PROBE_DECLARE(net_uuid, packet, ,	destroy);


#endif // _SYS_NET_UUID_KDTRACE_H_
