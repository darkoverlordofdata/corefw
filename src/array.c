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
#include <stdint.h>
#include <assert.h>
#include <stdio.h>

#include "object.h"
#include "array.h"
#include "hash.h"
#include "string.h"

struct __CFArray {
	struct __CFObject obj;
	void **data;
	size_t size;
};

static bool
ctor(void *ptr, va_list args)
{
	CFArrayRef array = ptr;
	void *obj;

	array->data = NULL;
	array->size = 0;

	while ((obj = va_arg(args, void*)) != NULL)
		if (!CFArrayPush(array, obj))
			return false;

	return true;
}

static void
dtor(void *ptr)
{
	CFArrayRef array = ptr;
	size_t i;

	for (i = 0; i < array->size; i++)
		CFUnref(array->data[i]);

	if (array->data != NULL)
		free(array->data);
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFObjectRef obj2 = ptr2;
	CFArrayRef array1, array2;
	size_t i;

	if (obj2->cls != CFArray)
		return false;

	array1 = ptr1;
	array2 = ptr2;

	if (array1->size != array2->size)
		return false;

	for (i = 0; i < array1->size; i++)
		if (CFEqual(array1->data[i], array2->data[i]))
			return false;

	return true;
}

static uint32_t
hash(void *ptr)
{
	CFArrayRef array = ptr;
	size_t i;
	uint32_t hash;

	CF_HASH_INIT(hash);

	for (i = 0; i < array->size; i++)
		CF_HASH_ADD_HASH(hash, CFHash(array->data[i]));

	CF_HASH_FINALIZE(hash);

	return hash;
}

static void*
copy(void *ptr)
{
	CFArrayRef array = ptr;
	CFArrayRef new;
	size_t i;

	if ((new = CFNew(CFArray, (void*)NULL)) == NULL)
		return NULL;

	if ((new->data = malloc(sizeof(void*) * array->size)) == NULL) {
		CFUnref(new);
		return NULL;
	}
	new->size = array->size;

	for (i = 0; i < array->size; i++)
		new->data[i] = CFRef(array->data[i]);

	return new;
}

static CFStringRef
toString(void *ptr)
{
	uint32_t h = hash(ptr);

   	int len = snprintf(NULL, 0, "CFArray: %u", h);
    char *s = malloc(len+1);
    if (s == NULL) return NULL;
	snprintf(s, len, "%u", h);
    CFStringRef str = CFCreate(CFString, s);
    free(s);
    return str;
	
}

void*
CFArrayGet(CFArrayRef array, size_t index)
{
	if (index >= array->size)
		return NULL;

	return array->data[index];
}

size_t
CFArraySize(CFArrayRef array)
{
	return array->size;
}

bool
CFArraySet(CFArrayRef array, size_t index, void *ptr)
{
	CFObjectRef obj = ptr;
	CFObjectRef old;

	if (index >= array->size)
		return false;

	CFRef(obj);
	old = array->data[index];
	array->data[index] = obj;
	CFUnref(old);

	return true;
}

bool
CFArrayPush(CFArrayRef array, void *ptr)
{
	CFObjectRef obj = ptr;
	void **new;

	if (array->data == NULL)
		new = malloc(sizeof(void*));
	else
		new = realloc(array->data, sizeof(void*) * (array->size + 1));

	if (new == NULL)
		return false;

	new[array->size] = CFRef(obj);

	array->data = new;
	array->size++;

	return true;
}

void*
CFArrayLast(CFArrayRef array)
{
	if (array->size == 0)
		return NULL;

	return array->data[array->size - 1];
}

bool
CFArrayPop(CFArrayRef array)
{
	void **new;
	void *last;

	if (array->size == 0)
		return NULL;

	if (array->size == 1) {
		CFUnref(array->data[0]);
		free(array->data);
		array->data = NULL;
		array->size = 0;
		return true;
	}

	last = array->data[array->size - 1];

	new = realloc(array->data, sizeof(void*) * (array->size - 1));
	if (new == NULL)
		return false;

	CFUnref(last);

	array->data = new;
	array->size--;

	return true;
}

bool
CFArrayContains(CFArrayRef array, void *ptr)
{
	size_t i;

	for (i = 0; i < array->size; i++)
		if (CFEqual(array->data[i], ptr))
			return true;

	return false;
}

bool
CFArrayContainsPtr(CFArrayRef array, void *ptr)
{
	size_t i;

	for (i = 0; i < array->size; i++)
		if (array->data[i] == ptr)
			return true;

	return false;
}

size_t
CFArrayFind(CFArrayRef array, void *ptr)
{
	size_t i;

	for (i = 0; i < array->size; i++)
		if (CFEqual(array->data[i], ptr))
			return i;

	return SIZE_MAX;
}

size_t
CFArrayFindPtr(CFArrayRef array, void *ptr)
{
	size_t i;

	for (i = 0; i < array->size; i++)
		if (array->data[i] == ptr)
			return i;

	return SIZE_MAX;
}

static struct __CFClass class = {
	.name = "CFArray",
	.size = sizeof(struct __CFArray),
	.ctor = ctor,
	.dtor = dtor,
	.equal = equal,
	.hash = hash,
	.copy = copy
};
CFClassRef CFArray = &class;
