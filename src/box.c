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
#include "box.h"
#include "string.h"

struct CFWBox {
	CFWObject obj;
	void *ptr;
	uint32_t type;
	bool free;
};

static bool
ctor(void *ptr, va_list args)
{
	CFWBox *box = ptr;

	box->ptr = va_arg(args, void*);
	box->type = va_arg(args, uint32_t);
	box->free = va_arg(args, int);

	return true;
}

static void
dtor(void *ptr)
{
	CFWBox *box = ptr;

	if (box->free)
		free(box->ptr);
}

static CFWString*
toString(void *ptr)
{
	uint32_t h = cfw_hash(ptr);

   	int len = snprintf(NULL, 0, "Box: %u", h);
    char *s = malloc(len+1);
    if (s == NULL) return NULL;
	snprintf(s, len, "%u", h);
    CFWString *str = cfw_create(cfw_string, s);
    free(s);
    return str;
	
}


void*
cfw_box_ptr(CFWBox *box)
{
	return box->ptr;
}

uint32_t
cfw_box_type(CFWBox *box)
{
	return box->type;
}

static CFWClass class = {
	.name = "CFWBox",
	.size = sizeof(CFWBox),
	.ctor = ctor,
	.dtor = dtor,
	.toString = toString
};
CFWClass *cfw_box = &class;
