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

#include "opt_no_net_uuid_tracing.h"
#include <net/net_uuid.h>
#include <sys/malloc.h>
#include <sys/queue.h>
#include <sys/param.h>
#include <sys/sdt.h>

#ifdef NO_NET_UUID_TRACING
// Define macros as empty

#define NET_UUID_PROBE_STR(mod, probe, t0, mt0)
#define NET_UUID_PROBE_STR_W_ADDRS(mod, probe, t0, mt0)
#define NET_UUID_PROBE2_STR(mod, probe, t0, mt0, arg1)
#define NET_UUID_PROBE2_STR_STR(mod, probe, t0, mt0, t1, mt1)

#else // NO_NET_UUID_TRACING

#define NET_UUID_PROBE_STR(mod, probe, t0, mt0)			do {	\
	char *str = net_uuid_get_uuid_str(t0, mt0);			\
	SDT_PROBE1(net_uuid, mod, , probe, str);			\
	free(str, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE_STR_W_ADDRS(mod, probe, t0, mt0)		do {	\
	char *str = net_uuid_get_uuid_str(t0, mt0);			\
	SDT_PROBE2(net_uuid, mod, , probe, str, mt0);			\
	free(str, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE2_STR_ADDRS(mod, probe, t0, mt0, m1)	do {	\
	char *str = net_uuid_get_uuid_str(t0, mt0);			\
	SDT_PROBE2(net_uuid, mod, , probe, str, m1);			\
	free(str, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE2_STR(mod, probe, t0, mt0, arg1)		do {	\
	char *str = net_uuid_get_uuid_str(t0, mt0);			\
	SDT_PROBE2(net_uuid, mod, , probe, str, arg1);			\
	free(str, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE2_UUID_STR(mod, probe, uuid0, arg1)	do {	\
	char *str = net_uuid_get_uuid_str_uuid(uuid0);			\
	SDT_PROBE2(net_uuid, mod, , probe, str, arg1);			\
	free(str, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE2_STR_UUID_STR(mod, probe, t0, mt0, uuid1) do {	\
	char *str0 = net_uuid_get_uuid_str(t0, mt0);			\
	char *str1 = net_uuid_get_uuid_str_uuid(uuid1);			\
	SDT_PROBE2(net_uuid, mod, , probe, str0, str1);			\
	free(str0, M_TEMP);						\
	free(str1, M_TEMP);						\
} while (0)

#define NET_UUID_PROBE2_STR_STR(mod, probe, t0, mt0, t1, mt1)	do {	\
	char *str0 = net_uuid_get_uuid_str(t0, mt0);			\
	char *str1 = net_uuid_get_uuid_str(t1, mt1);			\
	SDT_PROBE2(net_uuid, mod, , probe, str0, str1);			\
	free(str0, M_TEMP);						\
	free(str1, M_TEMP);						\
} while (0)

SDT_PROVIDER_DECLARE(net_uuid);

SDT_PROBE_DECLARE(net_uuid, mem, ,	alloc);

SDT_PROBE_DECLARE(net_uuid, packet, ,	trace__start);
SDT_PROBE_DECLARE(net_uuid, packet, ,	trace__stop);
SDT_PROBE_DECLARE(net_uuid, packet, ,	fragment);
SDT_PROBE_DECLARE(net_uuid, packet, ,	drop);
SDT_PROBE_DECLARE(net_uuid, packet, ,	from__socket);
SDT_PROBE_DECLARE(net_uuid, packet, ,	to__socket);

SDT_PROBE_DECLARE(net_uuid, packet, ,	to__subsys);

SDT_PROBE_DECLARE(net_uuid, socket, ,	create);

#endif // NO_NET_UUID_TRACING
#endif // _SYS_NET_UUID_KDTRACE_H_
