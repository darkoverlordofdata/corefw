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

#include "CFObject.h"
#include "CFInt.h"
#include "CFRefPool.h"
#include "CFString.h"

struct __CFInt {
	struct __CFObject obj;
	intmax_t value;
};

static bool
ctor(void *ptr, va_list args)
{
	CFIntRef integer = ptr;

	integer->value = va_arg(args, intmax_t);

	return true;
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFObjectRef obj2 = ptr2;
	CFIntRef int1, int2;

	if (obj2->cls != CFInt)
		return false;

	int1 = ptr1;
	int2 = ptr2;

	return (int1->value == int2->value);
}

static uint32_t
hash(void *ptr)
{
	CFIntRef integer = ptr;

	return (uint32_t)integer->value;
}

static void*
copy(void *ptr)
{
	return CFRef(ptr);
}


static CFStringRef
toString(void* ptr)
{
 	CFIntRef integer = ptr;
	
   	int len = snprintf(NULL, 0, "%jd", integer->value);
    char *s = malloc(len+1);
    if (s == NULL) return NULL;
	snprintf(s, len, "%jd", integer->value);
    CFStringRef str = CFCreate(CFString, s);
    free(s);
    return str;
	
}

intmax_t
CFInt_value(CFIntRef integer)
{
	return integer->value;
}

static struct __CFClass class = {
	.name = "CFInt",
	.size = sizeof(struct __CFInt),
	.ctor = ctor,
	.equal = equal,
	.hash = hash,
	.copy = copy,
	.toString = toString
};
CFClassRef CFInt = &class;

