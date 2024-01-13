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
#include <string.h>

#include "stream.h"

#define BUFFER_SIZE 4096

size_t strlen(const char *str);
void * memcpy ( void * destination, const void * source, size_t num );

static bool
ctor(void *ptr, va_list args)
{
	CFStreamRef stream = ptr;

	stream->ops = NULL;
	stream->cache = NULL;
	stream->cache_len = 0;

	return true;
}

static void
dtor(void *ptr)
{
	CFStreamClose(ptr);
}

static CFStringRef
toString(void *ptr)
{
	CFObjectRef this = ptr;
	uint32_t h = this->cls->hash(ptr);

   	int len = snprintf(NULL, 0, "CFArray: %u", h);
    char *s = malloc(len+1);
    if (s == NULL) return NULL;
	snprintf(s, len, "%u", h);
    CFStringRef str = CFCreate(CFString, s);
    free(s);
    return str;
	
}

ssize_t
CFStream_read(void *ptr, void *buf, size_t len)
{
	CFStreamRef stream = ptr;
	ssize_t ret;

	if (stream == NULL || stream->ops == NULL)
		return -1;

	if (stream->cache == NULL) {
		if ((ret = stream->ops->read(stream, buf, len)) < -1)
			ret = -1;

		return ret;
	}

	if (len >= stream->cache_len) {
		ret = stream->cache_len;

		memcpy(buf, stream->cache, stream->cache_len);

		free(stream->cache);
		stream->cache = NULL;
		stream->cache_len = 0;

		return ret;
	} else {
		char *tmp;

		if ((tmp = malloc(stream->cache_len - len)) == NULL)
			return -1;
		memcpy(tmp, stream->cache + len, stream->cache_len - len);
		memcpy(buf, stream->cache, len);

		free(stream->cache);
		stream->cache = tmp;
		stream->cache_len -= len;

		return len;
	}
}

CFStringRef
CFStreamReadLine(void *ptr)
{
	CFStreamRef stream = ptr;
	CFStringRef ret;
	char *buf, *ret_str, *new_cache;
	ssize_t buf_len;
	size_t i, ret_len;

	/* Look if there is a line or \0 in our cache */
	if (stream->cache != NULL) {
		for (i = 0; i < stream->cache_len; i++) {
			if (stream->cache[i] == '\n' ||
			    stream->cache[i] == '\0') {
				ret_len = i;
				if (i > 0 && stream->cache[i - 1] == '\r')
					ret_len--;

				ret_str = CFStrndup(stream->cache, ret_len);
				if (ret_str == NULL)
					return NULL;

				ret = CFCreate(CFString, (void*)NULL);
				if (ret == NULL) {
					free(ret_str);
					return NULL;
				}
				CFStringSetNocopy(ret, ret_str, ret_len);

				if (stream->cache_len > i + 1) {
					if ((new_cache = malloc(
					    stream->cache_len - i - 1)) == NULL)
						return NULL;
					memcpy(new_cache, stream->cache + i + 1,
					    stream->cache_len - i - 1);
				} else
					new_cache = CFStrdup("");

				free(stream->cache);
				stream->cache = new_cache;
				stream->cache_len -= i + 1;

				return ret;
			}
		}
	}

	/* Read and see if we get a newline or \0 */

	if ((buf = malloc(BUFFER_SIZE)) == NULL)
		return NULL;

	for (;;) {
		if (stream->ops->at_end(stream)) {
			free(buf);

			if (stream->cache == NULL)
				return NULL;

			ret_len = stream->cache_len;

			if (ret_len > 0 && stream->cache[ret_len - 1] == '\r')
				ret_len--;

			ret_str = CFStrndup(stream->cache, ret_len);
			if (ret_str == NULL)
				return NULL;

			ret = CFCreate(CFString, (void*)NULL);
			if (ret == NULL) {
				free(ret_str);
				return NULL;
			}
			CFStringSetNocopy(ret, ret_str, ret_len);

			free(stream->cache);
			stream->cache = NULL;
			stream->cache_len = 0;

			return ret;
		}

		buf_len = stream->ops->read(stream, buf, BUFFER_SIZE);
		if (buf_len == -1) {
			free(buf);
			return NULL;
		}

		/* Look if there's a newline or \0 */
		for (i = 0; i < buf_len; i++) {
			if (buf[i] == '\n' || buf[i] == '\0') {
				ret_len = stream->cache_len + i;

				if ((ret_str = malloc(ret_len + 1)) == NULL) {
					/*
					 * FIXME: We lost the current buffer.
					 *	  Mark the stream as broken?
					 */
					free(buf);
					return NULL;
				}
				memcpy(ret_str, stream->cache,
				    stream->cache_len);
				memcpy(ret_str + stream->cache_len, buf, i);
				if (ret_len > 0 && ret_str[ret_len - 1] == '\r')
					ret_len--;
				ret_str[ret_len] = '\0';

				ret = CFCreate(CFString, (void*)NULL);
				if (ret == NULL) {
					free(buf);
					free(ret_str);
					return NULL;
				}
				CFStringSetNocopy(ret, ret_str, ret_len);

				if (buf_len > i + 1) {
					new_cache = malloc(buf_len - i - 1);
					if (new_cache == NULL) {
						free(buf);
						return NULL;
					}
					memcpy(new_cache, buf + i + 1,
					    buf_len - i - 1);
				} else
					new_cache = CFStrdup("");

				free(stream->cache);
				stream->cache = new_cache;
				stream->cache_len = buf_len - i - 1;

				free(buf);
				return ret;
			}
		}

		/* There was no newline or \0 */
		if (stream->cache_len + buf_len > 0) {
			new_cache = realloc(stream->cache,
			    stream->cache_len + buf_len);
			if (new_cache == NULL) {
				free(buf);
				return NULL;
			}
			memcpy(new_cache + stream->cache_len, buf, buf_len);
		} else {
			free(stream->cache);
			new_cache = CFStrdup("");
		}

		stream->cache = new_cache;
		stream->cache_len += buf_len;
	}
}

bool
CFStreamWrite(void *ptr, const void *buf, size_t len)
{
	CFStreamRef stream = ptr;

	if (stream == NULL || stream->ops == NULL)
		return false;

	return stream->ops->write(stream, buf, len);
}

bool
CFStreamWriteString(void *ptr, const char *str)
{
	return CFStreamWrite(ptr, str, strlen(str));
}

bool
CFStreamWriteLine(void *ptr, const char *str)
{
	char *tmp;
	size_t len;

	len = strlen(str);

	if ((tmp = malloc(len + 2)) == NULL)
		return false;

	memcpy(tmp, str, len);
	tmp[len] = '\n';
	tmp[len + 1] = '\0';

	if (!CFStreamWrite(ptr, tmp, len + 1)) {
		free(tmp);
		return false;
	}

	free(tmp);
	return true;
}

bool
CFStreamAtEnd(void *ptr)
{
	CFStreamRef stream = ptr;

	if (stream == NULL || stream->ops == NULL)
		return true;

	if (stream->cache != NULL)
		return false;

	return stream->ops->at_end(stream);
}

void
CFStreamClose(void *ptr)
{
	CFStreamRef stream = ptr;

	if (stream == NULL || stream->ops == NULL)
		return;

	stream->ops->close(stream);
}

static struct __CFClass class = {
	.name = "CFStream",
	.size = sizeof(struct __CFStream),
	.ctor = ctor,
	.dtor = dtor,
	.toString = toString
};
CFClassRef CFStream = &class;
