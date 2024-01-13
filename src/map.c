/*
 * Copyright (c) 2012, Jonathan Schleifer <js@webkeks.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "object.h"
#include "map.h"
#include "hash.h"
#include "string.h"

static struct bucket {
	CFObjectRef key, obj;
	uint32_t hash;
} deleted = { NULL, NULL, 0 };

struct __CFMap {
	struct __CFObject obj;
	struct bucket **data;
	uint32_t size;
	size_t items;
};

static bool
ctor(void *ptr, va_list args)
{
	CFMapRef map = ptr;
	void *key;

	map->data = NULL;
	map->size = 0;
	map->items = 0;

	while ((key = va_arg(args, void*)) != NULL)
		if (!CFMapSet(map, key, va_arg(args, void*)))
			return false;

	return true;
}

static void
dtor(void *ptr)
{
	CFMapRef map = ptr;
	uint32_t i;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			CFUnref(map->data[i]->key);
			CFUnref(map->data[i]->obj);
			free(map->data[i]);
		}
	}

	if (map->data != NULL)
		free(map->data);
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFObjectRef obj2 = ptr2;
	CFMapRef map1, map2;
	uint32_t i;

	if (obj2->cls != CFMap)
		return false;

	map1 = ptr1;
	map2 = ptr2;

	if (map1->items != map2->items)
		return false;

	for (i = 0; i < map1->size; i++)
		if (map1->data[i] != NULL && map1->data[i] != &deleted &&
		    !CFEqual(CFMapGet(map2, map1->data[i]->key),
		    map1->data[i]->obj))
			return false;

	return true;
}

static uint32_t
hash(void *ptr)
{
	CFMapRef map = ptr;
	uint32_t i, hash = 0;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			hash += map->data[i]->hash;
			hash += CFHash(map->data[i]->obj);
		}
	}

	return hash;
}

static void*
copy(void *ptr)
{
	CFMapRef map = ptr;
	CFMapRef new;
	uint32_t i;

	if ((new = CFNew(CFMap, (void*)NULL)) == NULL)
		return NULL;

	// NOLINTNEXTLINE
	if ((new->data = malloc(sizeof(*new->data) * map->size)) == NULL)
		return NULL;
	new->size = map->size;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			struct bucket *bucket;

			if ((bucket = malloc(sizeof(*bucket))) == NULL)
				return NULL;

			bucket->key = CFRef(map->data[i]->key);
			bucket->obj = CFRef(map->data[i]->obj);
			bucket->hash = map->data[i]->hash;

			new->data[i] = bucket;
		} else
			new->data[i] = NULL;
	}

	return new;
}

static CFStringRef
toString(void *ptr)
{
	uint32_t h = CFHash(ptr);

   	int len = snprintf(NULL, 0, "CFMap: %u", h);
    char *s = malloc(len+1);
    if (s == NULL) return NULL;
	snprintf(s, len, "%u", h);
    CFStringRef str = CFCreate(CFString, s);
    free(s);
    return str;
	
}

bool
resize(CFMapRef map, uint32_t items)
{
	size_t fullness = items * 4 / map->size;
	struct bucket **ndata;
	uint32_t i, nsize;

	if (items > UINT32_MAX)
		return false;

	if (fullness >= 3)
		nsize = map->size << 1;
	else if (fullness <= 1)
		nsize = map->size >> 1;
	else
		return true;

	if (nsize == 0)
		return false;

	// NOLINTNEXTLINE
	if ((ndata = malloc(nsize * sizeof(*ndata))) == NULL)
		return false;

	for (i = 0; i < nsize; i++)
		ndata[i] = NULL;

	for (i = 0; i < map->size; i++) {
		if (map->data[i] != NULL && map->data[i] != &deleted) {
			uint32_t j, last;

			last = nsize;

			j = map->data[i]->hash & (nsize - 1);
			for (; j < last && ndata[j] != NULL; j++);

			/* In case the last bucket is already used */
			if (j >= last) {
				last = map->data[i]->hash & (nsize - 1);

				for (j = 0; j < last && ndata[j] != NULL; j++);
			}

			if (j >= last) {
				free(ndata);
				return false;
			}

			ndata[j] = map->data[i];
		}
	}

	free(map->data);
	map->data = ndata;
	map->size = nsize;

	return true;
}

size_t
CFMapSize(CFMapRef map)
{
	return map->items;
}

void*
CFMapGet(CFMapRef map, void *key)
{
	uint32_t i, hash, last;

	if (key == NULL)
		return NULL;

	hash = CFHash(key);
	last = map->size;

	for (i = hash & (map->size - 1);
	    i < last && map->data[i] != NULL; i++) {
		if (map->data[i] == &deleted)
			continue;

		if (CFEqual(map->data[i]->key, key))
			return map->data[i]->obj;
	}

	if (i < last)
		return NULL;

	/* In case the last bucket is already used */
	last = hash & (map->size - 1);

	for (i = 0; i < last && map->data[i] != NULL; i++) {
		if (map->data[i] == &deleted)
			continue;

		if (CFEqual(map->data[i]->key, key))
			return map->data[i]->obj;
	}

	return NULL;
}

void*
CFMapGetC(CFMapRef map, const char *key)
{
	CFStringRef str;
	void *ret;

	if ((str = CFNew(CFString, key)) == NULL)
		return NULL;

	ret = CFMapGet(map, str);

	CFUnref(str);

	return ret;
}

bool
CFMapSet(CFMapRef map, void *key, void *obj)
{
	uint32_t i, hash, last;

	if (key == NULL)
		return false;

	if (map->data == NULL) {
		// NOLINTNEXTLINE
		if ((map->data = malloc(sizeof(*map->data))) == NULL)
			return false;

		map->data[0] = NULL;
		map->size = 1;
		map->items = 0;
	}

	hash = CFHash(key);
	last = map->size;

	for (i = hash & (map->size - 1);
	    i < last && map->data[i] != NULL; i++) {
		if (map->data[i] == &deleted)
			continue;

		if (CFEqual(map->data[i]->key, key))
			break;
	}

	/* In case the last bucket is already used */
	if (i >= last) {
		last = hash & (map->size - 1);

		for (i = 0; i < last && map->data[i] != NULL; i++) {
			if (map->data[i] == &deleted)
				continue;

			if (CFEqual(map->data[i]->key, key))
				break;
		}
	}

	/* Key not in dictionary */
	if (i >= last || map->data[i] == NULL || map->data[i] == &deleted ||
	    !CFEqual(map->data[i]->key, key)) {
		struct bucket *bucket;

		if (obj == NULL)
			return true;

		if (!resize(map, map->items + 1))
			return false;

		last = map->size;

		for (i = hash & (map->size - 1); i < last &&
		    map->data[i] != NULL && map->data[i] != &deleted; i++);

		/* In case the last bucket is already used */
		if (i >= last) {
			last = hash & (map->size - 1);

			for (i = 0; i < last && map->data[i] != NULL &&
			    map->data[i] != &deleted; i++);
		}

		if (i >= last)
			return false;

		if ((bucket = malloc(sizeof(*bucket))) == NULL)
			return false;

		if ((bucket->key = CFCopy(key)) == NULL) {
			free(bucket);
			return false;
		}

		bucket->obj = CFRef(obj);
		bucket->hash = CFHash(key);

		map->data[i] = bucket;
		map->items++;

		return true;
	}

	if (obj != NULL) {
		void *old = map->data[i]->obj;
		map->data[i]->obj = CFRef(obj);
		CFUnref(old);
	} else {
		CFUnref(map->data[i]->key);
		CFUnref(map->data[i]->obj);

		free(map->data[i]);
		map->data[i] = &deleted;

		map->items--;

		if (!resize(map, map->items))
			return false;
	}

	return true;
}

bool
CFMapSetC(CFMapRef map, const char *key, void *obj)
{
	CFStringRef str;
	bool ret;

	if ((str = CFNew(CFString, key)) == NULL)
		return false;

	ret = CFMapSet(map, str, obj);

	CFUnref(str);

	return ret;
}

void
CFMapIter(CFMapRef map, CFMapIter_t *iter)
{
	iter->_map = map;
	iter->_pos = 0;

	CFMapIterNext(iter);
}

void
CFMapIterNext(CFMapIter_t *iter)
{
	CFMapRef map = iter->_map;

	for (; iter->_pos < map->size &&
	    (map->data[iter->_pos] == NULL ||
	    map->data[iter->_pos] == &deleted); iter->_pos++);

	if (iter->_pos < map->size) {
		iter->key = map->data[iter->_pos]->key;
		iter->obj = map->data[iter->_pos]->obj;
		iter->_pos++;
	} else {
		iter->key = NULL;
		iter->obj = NULL;
	}
}

static struct __CFClass class = {
	.name = "CFMap",
	.size = sizeof(struct __CFMap),
	.ctor = ctor,
	.dtor = dtor,
	.equal = equal,
	.hash = hash,
	.copy = copy,
	.toString = toString
};
CFClassRef CFMap = &class;
