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

#include "CFObject.h"
#include "CFBitVector.h"
#include "CFHash.h"
#include <limits.h>		/* for CHAR_BIT */

#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(a, b) ((a)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(a, b) ((a)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(a, b) ((a)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)


struct __CFBitVector {
	struct __CFObject obj;
	char *data;
	size_t length;
	size_t alloc;
};

static bool
ctor(void *ptr, va_list args)
{
	CFBitVectorRef this = ptr;
	void *obj;

	this->length = (size_t)va_arg(args, int);
	this->alloc = BITNSLOTS(this->length);
	this->data = malloc(this->alloc + 1);

	return true;
}

static void
dtor(void *ptr)
{
	CFBitVectorRef this = ptr;
	if (this->data != NULL)
		free(this->data);
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFObjectRef obj1 = ptr1;
	CFObjectRef obj2 = ptr2;
	
	if (obj1->cls != CFBitVector)
		return false;
	if (obj2->cls != CFBitVector)
		return false;

	CFBitVectorRef this1 = ptr1;
	CFBitVectorRef this2 = ptr2;

	if (this1->length != this2->length)
		return false;

	for (size_t i = 0; i < this1->length; i++)
		if (this1->data[i] != this2->data[i])
			return false;

	return true;
}

static uint32_t
hash(void *ptr)
{
	CFBitVectorRef this = ptr;
	size_t i;
	uint32_t hash;

	CF_HASH_INIT(hash);

	for (i = 0; i < this->length; i++)
		CF_HASH_ADD_HASH(hash, (uint32_t)this->data[i]);

	CF_HASH_FINALIZE(hash);

	return hash;
}

static void*
copy(void *ptr)
{
	CFBitVectorRef this = ptr;
	CFBitVectorRef new;
	size_t i;

	if ((new = CFNew(CFBitVector, (void*)NULL)) == NULL)
		return NULL;

	if ((new->data = malloc(sizeof(void*) * this->length)) == NULL) {
		CFUnref(new);
		return NULL;
	}
	new->length = this->length;

	for (i = 0; i < this->length; i++)
		new->data[i] = this->data[i];

	return new;
}

size_t
CFBitVectorGetCount(CFBitVectorRef this)
{
	return this->length;
}


void
CFBitVectorSetBitAtIndex(CFBitVectorRef this, size_t index, bool value)
{
	if (value) 
		BITSET(this->data, index);
	else
		BITCLEAR(this->data, index);
}

bool
CFBitVectorGetBitAtIndex(CFBitVectorRef this, size_t index)
{
	if (BITTEST(this->data, index)) 
		return true;
	else
	 	return false;
}

static struct __CFClass class = {
	.name = "CFBitVector",
	.size = sizeof(struct __CFBitVector),
	.ctor = ctor,
	.dtor = dtor,
	.equal = equal,
	.hash = hash,
	.copy = copy
};
CFClassRef CFBitVector = &class;
