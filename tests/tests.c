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

#include "object.h"
#include <stdio.h>
#include <corefw.h>

#define CF_ENDOFLIST NULL
#define CF_REMOVE NULL

static inline void* CFBoolCreate(int value) {
	return CFCreate(CFBool, value);
}
static inline void* CFDoubleCreate(double value) {
	return CFCreate(CFDouble, value);
}
static inline void* CFIntCreate(int64_t value) {
	return CFCreate(CFInt, value);
}
static inline void* CFStringCreate(const char* value) {
	return CFCreate(CFString, value);
}


static void
print_map(CFMapRef map)
{
	CFMapIter_t iter;

	CFMapIter(map, &iter);

	fputs("{\n", stdout);

	CFStringRef str;
	while (iter.key != NULL) {
		str = CFToString(iter.obj);
		printf("\t%s = %s\n", CFStringC(iter.key), CFStringC(str));

		CFMapIterNext(&iter);
	}

	fputs("}\n", stdout);
}

int
main(int argc, char *argv[])
{
	CFRefPoolRef pool = CFNew(CFRefPool);

	CFArrayRef array = CFCreate(CFArray,
	    CFStringCreate("Hallo"),
	    CFStringCreate("Welt"),
	    CFStringCreate("!"),
		CF_ENDOFLIST);

	CFStringRef str = CFNew(CFString, (void*)NULL);

	for (int i = 0; i < CFArraySize(array); i++) {
		CFStringAppend(str, CFArrayGet(array, i));
	}

	puts(CFStringC(str));

	CFStringRef str2 = CFStringCreate("ll");
	printf("%zd\n", CFStringFind(str, str2, CFRangeAll));

	CFUnref(str);

	CFMapRef map = CFCreate(CFMap,
	    CFStringCreate("Hallo"),		CFStringCreate("Welt!"),
	    CFStringCreate("int"), 		CFIntCreate(1234), 
	    CFStringCreate("double"),		CFDoubleCreate(3.1415), 
	    CFStringCreate("Test"),		CFStringCreate("success!"),
		CF_ENDOFLIST);

	print_map(map);

	CFMapSet(map, CFStringCreate("Hallo"), CFStringCreate("Test"));

	print_map(map);

	CFMapSet(map, CFStringCreate("Hallo"), CF_REMOVE);
	print_map(map);

	CFUnref(pool);

	return 0;
}
