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

#include <stdio.h>

#include "object.h"
#include "string.h"
#include "array.h"
#include "map.h"

int
main()
{
	CFWString *s[3];
	CFWArray *a;
	CFWMap *m;
	size_t i;

	s[0] = cfw_new(cfw_string, "Hallo");
	s[1] = cfw_new(cfw_string, " Welt");
	s[2] = cfw_new(cfw_string, "!");

	a = cfw_new(cfw_array, s[0], s[1], s[2], NULL);

	cfw_unref(s[0]);
	cfw_unref(s[1]);
	cfw_unref(s[2]);

	s[0] = cfw_new(cfw_string, NULL);

	for (i = 0; i < cfw_array_size(a); i++)
		cfw_string_append(s[0], cfw_array_get(a, i));

	cfw_unref(a);

	puts(cfw_string_c(s[0]));

	s[1] = cfw_new(cfw_string, "ll");
	printf("%zd\n", cfw_string_find(s[0], s[1], cfw_range_all));
	cfw_unref(s[1]);

	cfw_unref(s[0]);

	s[0] = cfw_new(cfw_string, "Hallo");
	s[1] = cfw_new(cfw_string, "Welt!");

	m = cfw_new(cfw_map, s[0], s[1], NULL);

	cfw_unref(s[1]);

	puts(cfw_string_c(cfw_map_get(m, s[0])));

	s[1] = cfw_new(cfw_string, "Test");
	cfw_map_set(m, s[0], s[1]);
	cfw_unref(s[1]);

	puts(cfw_string_c(cfw_map_get(m, s[0])));

	cfw_map_set(m, s[0], NULL);
	printf("%p\n", cfw_map_get(m, s[0]));

	cfw_unref(s[0]);
	cfw_unref(m);

	return 0;
}
