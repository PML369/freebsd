/* $FreeBSD: head/lib/libcompiler_rt/__sync_fetch_and_or_8.c 249968 2013-04-27 04:56:02Z ed $ */
#define	NAME		__sync_fetch_and_or_8
#define	TYPE		int64_t
#define	CMPSET		atomic_cmpset_64
#define	EXPRESSION	t | value

#include "__sync_fetch_and_op_n.h"
