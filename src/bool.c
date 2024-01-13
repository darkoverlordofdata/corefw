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
#include "bool.h"
#include "string.h"

struct __CFBool {
	struct __CFObject obj;
	bool value;
};

static bool
ctor(void *ptr, va_list args)
{
	CFBoolRef boolean = ptr;

	boolean->value = va_arg(args, int);

	return true;
}

static bool
equal(void *ptr1, void *ptr2)
{
	CFObjectRef obj2 = ptr2;
	CFBoolRef boolean1, boolean2;

	if (obj2->cls != CFBool)
		return false;

	boolean1 = ptr1;
	boolean2 = ptr2;

	return (boolean1->value == boolean2->value);
}

static uint32_t
hash(void *ptr)
{
	CFBoolRef boolean = ptr;

	return (uint32_t)boolean->value;
}

static void*
copy(void *ptr)
{
	return CFRef(ptr);
}

static CFStringRef
toString(void* ptr)
{
 	CFBoolRef boolean = ptr;
	
   	int len = snprintf(NULL, 0, "%s", boolean->value ? "true": "false");
    char *s = malloc(len+1);
    if (s == NULL) return NULL;
	snprintf(s, len, "%s", boolean->value ? "true": "false");
    CFStringRef str = CFCreate(CFString, s);
    free(s);
    return str;
	
}

bool
CFBoolValue(CFBoolRef boolean)
{
	return boolean->value;
}

static struct __CFClass class = {
	.name = "CFBool",
	.size = sizeof(struct __CFBool),
	.ctor = ctor,
	.equal = equal,
	.hash = hash,
	.copy = copy,
	.toString = toString
};
CFClassRef CFBool = &class;
