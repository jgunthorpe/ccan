#ifndef CCAN_TDB2_H
#define CCAN_TDB2_H

/* 
   Unix SMB/CIFS implementation.

   trivial database library

   Copyright (C) Andrew Tridgell 1999-2004
   
     ** NOTE! The following LGPL license applies to the tdb
     ** library. This does NOT imply that all of Samba is released
     ** under the LGPL
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see <http://www.gnu.org/licenses/>.
*/

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef _SAMBA_BUILD_
/* For mode_t */
#include <sys/types.h>
/* For O_* flags. */
#include <sys/stat.h>
/* For sig_atomic_t. */
#include <signal.h>
/* For uint64_t */
#include <stdint.h>
#endif

/* flags to tdb_store() */
#define TDB_REPLACE 1		/* Unused */
#define TDB_INSERT 2 		/* Don't overwrite an existing entry */
#define TDB_MODIFY 3		/* Don't create an existing entry    */

/* flags for tdb_open() */
#define TDB_DEFAULT 0 /* just a readability place holder */
#define TDB_CLEAR_IF_FIRST 1
#define TDB_INTERNAL 2 /* don't store on disk */
#define TDB_NOLOCK   4 /* don't do any locking */
#define TDB_NOMMAP   8 /* don't use mmap */
#define TDB_CONVERT 16 /* convert endian (internal use) */
#define TDB_BIGENDIAN 32 /* header is big-endian (internal use) */
#define TDB_NOSYNC   64 /* don't use synchronous transactions */
#define TDB_SEQNUM   128 /* maintain a sequence number */
#define TDB_VOLATILE   256 /* Activate the per-hashchain freelist, default 5 */
#define TDB_ALLOW_NESTING 512 /* Allow transactions to nest */
#define TDB_DISALLOW_NESTING 1024 /* Disallow transactions to nest */

/* error codes */
enum TDB_ERROR {TDB_SUCCESS=0, TDB_ERR_CORRUPT, TDB_ERR_IO, TDB_ERR_LOCK, 
		TDB_ERR_OOM, TDB_ERR_EXISTS, TDB_ERR_NOLOCK, TDB_ERR_LOCK_TIMEOUT,
		TDB_ERR_NOEXIST, TDB_ERR_EINVAL, TDB_ERR_RDONLY,
		TDB_ERR_NESTING};

/* debugging uses one of the following levels */
enum tdb_debug_level {TDB_DEBUG_FATAL = 0, TDB_DEBUG_ERROR, 
		      TDB_DEBUG_WARNING, TDB_DEBUG_TRACE};

typedef struct tdb_data {
	unsigned char *dptr;
	size_t dsize;
} TDB_DATA;

#ifndef PRINTF_ATTRIBUTE
#if (__GNUC__ >= 3)
/** Use gcc attribute to check printf fns.  a1 is the 1-based index of
 * the parameter containing the format, and a2 the index of the first
 * argument. Note that some gcc 2.x versions don't handle this
 * properly **/
#define PRINTF_ATTRIBUTE(a1, a2) __attribute__ ((format (__printf__, a1, a2)))
#else
#define PRINTF_ATTRIBUTE(a1, a2)
#endif
#endif

struct tdb_context;

/* FIXME: Make typesafe */
typedef void (*tdb_logfn_t)(struct tdb_context *, enum tdb_debug_level, void *priv, const char *, ...) PRINTF_ATTRIBUTE(4, 5);
typedef uint64_t (*tdb_hashfn_t)(const void *key, size_t len, uint64_t seed,
				 void *priv);

enum tdb_attribute_type {
	TDB_ATTRIBUTE_LOG = 0,
	TDB_ATTRIBUTE_HASH = 1
};

struct tdb_attribute_base {
	enum tdb_attribute_type attr;
	union tdb_attribute *next;
};

struct tdb_attribute_log {
	struct tdb_attribute_base base; /* .attr = TDB_ATTRIBUTE_LOG */
	tdb_logfn_t log_fn;
	void *log_private;
};

struct tdb_attribute_hash {
	struct tdb_attribute_base base; /* .attr = TDB_ATTRIBUTE_HASH */
	tdb_hashfn_t hash_fn;
	void *hash_private;
};

union tdb_attribute {
	struct tdb_attribute_base base;
	struct tdb_attribute_log log;
	struct tdb_attribute_hash hash;
};
		
struct tdb_context *tdb_open(const char *name, int tdb_flags,
			     int open_flags, mode_t mode,
			     union tdb_attribute *attributes);

struct tdb_data tdb_fetch(struct tdb_context *tdb, struct tdb_data key);
int tdb_delete(struct tdb_context *tdb, struct tdb_data key);
int tdb_store(struct tdb_context *tdb, struct tdb_data key, struct tdb_data dbuf, int flag);
int tdb_close(struct tdb_context *tdb);
int tdb_check(struct tdb_context *tdb,
	      int (*check)(TDB_DATA key, TDB_DATA data, void *private_data),
	      void *private_data);

extern struct tdb_data tdb_null;

#ifdef  __cplusplus
}
#endif

#endif /* tdb2.h */
