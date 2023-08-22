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

#define CFW_ENDOFLIST NULL
#define CFW_REMOVE NULL

static inline void* cfw_create_bool(int value) {
	return cfw_create(cfw_bool, value);
}
static inline void* cfw_create_double(double value) {
	return cfw_create(cfw_double, value);
}
static inline void* cfw_create_int(int64_t value) {
	return cfw_create(cfw_int, value);
}
static inline void* cfw_create_string(const char* value) {
	return cfw_create(cfw_string, value);
}


static void
print_map(CFWMap *map)
{
	cfw_map_iter_t iter;

	cfw_map_iter(map, &iter);

	fputs("{\n", stdout);

	CFWString *str;
	while (iter.key != NULL) {
		str = cfw_toString(iter.obj);
		printf("\t%s = %s\n", cfw_string_c(iter.key), cfw_string_c(str));

		cfw_map_iter_next(&iter);
	}

	fputs("}\n", stdout);
}

int
main(int argc, char *argv[])
{
	CFWRefPool *pool = cfw_new(cfw_refpool);

	CFWArray *array = cfw_create(cfw_array,
	    cfw_create_string("Hallo"),
	    cfw_create_string("Welt"),
	    cfw_create_string("!"),
		CFW_ENDOFLIST);

	CFWString *str = cfw_new(cfw_string, (void*)NULL);

	for (int i = 0; i < cfw_array_size(array); i++) {
		cfw_string_append(str, cfw_array_get(array, i));
	}

	puts(cfw_string_c(str));

	CFWString *str2 = cfw_create_string("ll");
	printf("%zd\n", cfw_string_find(str, str2, cfw_range_all));

	cfw_unref(str);

	CFWMap *map = cfw_create(cfw_map,
	    cfw_create_string("Hallo"),		cfw_create_string("Welt!"),
	    cfw_create_string("int"), 		cfw_create_int(1234), 
	    cfw_create_string("double"),	cfw_create_double(3.1415), 
	    cfw_create_string("Test"),		cfw_create_string("success!"),
		CFW_ENDOFLIST);

	print_map(map);

	cfw_map_set(map, cfw_create_string("Hallo"), cfw_create_string("Test"));

	print_map(map);

	cfw_map_set(map, cfw_create_string("Hallo"), CFW_REMOVE);
	print_map(map);

	cfw_unref(pool);

	return 0;
}
