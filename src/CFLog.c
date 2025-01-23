/*
 * Copyright (c) 2018 Dark Overlord of Data
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
#include "CFLog.h"
#include "CFObject.h"
#include "CFArray.h"
#include "CFHash.h"
#include "CFString.h"

/**
 * Log
 * 
 * Writes output to stderr, with end of line.
 */
__attribute__((__format__ (__printf__, 1, 2)))
void CFLog (char* format, ...) {

    va_list args;
    va_start(args, format);
    CFvfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "\n");
}


__attribute__((__format__ (__printf__, 1, 2)))                          
char* CFsprintf(const char* format, ...) 
{
    va_list args1;
    va_list args2;
    
    va_start(args1, format);
    va_copy(args2, args1);  

    int len = CFvsnprintf(NULL, 0, format, args1);
    va_end(args1);
    if (len == 0) return "";
    char* str = (char*)calloc((len+1), sizeof(char));
    len = CFvsnprintf(str, len+1, format, args2);
    va_end(args2);
    return str;
}

void CFvfprintf(FILE* stream, const char* format, va_list _args) 
{
    va_list args1;
    va_list args2;
    
    va_copy(args1, _args);
    va_copy(args2, _args);  

    int len = CFvsnprintf(NULL, 0, format, args1);
    va_end(args1);
    char* str = (char*)calloc((len+1), sizeof(char));
    len = CFvsnprintf(str, len+1, format, args2);
    va_end(args2);
    fputs(str, stream);
    free(str);
    return;
}

int CFvsnprintf(char* str, size_t n, const char* format, va_list _args)  
{
    char *current; 
    char result[40];
    char spec[40];
    int i;
    int l;
    int c;
    char *s;
    CFObjectRef object;
    char *dst = str;
    bool update = (str != NULL);
    int length = 0;
    // char* types = "hlLzjt-+ 0\'#diuoxXfFeEgGaAcspn$";
    char* types = "hlLzjtdiuoxXfFeEgGaAcspn$";
    va_list arg; 
    va_copy(arg, _args);
    
    /**
     * decompose the format string into specifiers.
     * call sprintf for standard specifiers, and handle
     * new specifiers.
     */
    for (current = (char *)format; *current != '\0'; current++) { 

        while ( *current != '%' ) { 
            if (*current == '\0') return length;
            if (update) *dst++ = *current;
            current++; 
            length++;
        } 
        c = 0;
        memset(spec, 0, 40);
        spec[c++] = *current;
        current++;

        /** 
         * collect the specifier 
         * 
         *  %[flags][width][.precision][length]specifier 
         * 
         * Everything between the leading % and the specifier is collected
         * and passed on to stdlib sprintf. This is done to add a new
         * specifier, '$' for CF objects.
         * 
         */
        while ((*current != '\0') 
            && (strchr(types, *current) != NULL)) {
            spec[c++] = *current;
            current++;
        }
        /** Back up one, and point to the specifier */
        current--;

        switch (*current) { 
            /* flags */
        case '-':
        case '+':
        case ' ':
        case '0':
        case '\'':
        case '#':
            if (update) {
                *dst = *current;
                *dst++ = i;
                
            }
            current++;
            break;

            /* length */
        case 'h':   /* int from char */
        case 'l':   /* long or float */
        case 'L':   /* long double */
        case 'z':   /* size_t */
        case 'j':   /* intmax_t*/
        case 't':   /* ptrdiff_t */
            if (update) {
                *dst = *current;
                *dst++ = i;
            }
            if ((*current == 'h' && *current+1 == 'h') 
                ||
                (*current == 'l' && *current+1 == 'l')) {
                if (update) {
                    *dst = *current;
                    *dst++ = i;
                }
                current++;
            }
            current++;

            break;


        case 'c' :  /** char */
            i = va_arg(arg, int);    
            if (update) *dst++ = i;
            length++;
            break; 
                    
        case 'f' :  /** float */
        case 'F' : 
        case 'e' : 
        case 'E' : 
        case 'g' : 
        case 'G' : 
        case 'a' : 
        case 'A' : 
            l = sprintf(result, spec, va_arg(arg, double));
            if (update) dst = strncpy(dst, result, l) + l;
            length += l;
            break;

        case 'u':   /** unsigned */
            l = sprintf(result, spec, va_arg(arg, unsigned int));
            if (update) dst = strncpy(dst, result, l) + l;
            length += l;
            break;

        case 'd':   /** integer */
        case 'i': 
            l = sprintf(result, spec, va_arg(arg, int));
            if (update) dst = strncpy(dst, result, l) + l;
            length += l;
            break;

        case 'o':   /** octal */
            l = sprintf(result, spec, va_arg(arg, unsigned int));
            if (update) dst = strncpy(dst, result, l) + l;
            length += l;
            break;
                    
        case 's':   /** string */
            s = va_arg(arg, char *);       
            l = strlen(s);
            if (update) dst = strncpy(dst, s, l) + l;
            length += l;
            break; 
                    
        case 'x':   /** hex */
        case 'X':
            l = sprintf(result, spec, va_arg(arg, unsigned int));
            if (update) dst = strncpy(dst, result, l) + 1;
            length += l;
            break;

        case '$':   /** CFObject */
            object = va_arg(arg, CFObjectRef);   
            l = strlen(CFStringC(CFToString(object)));
            if (update) dst = strncpy(dst, CFStringC(CFToString(object)), l) + l;
            length += l;
            break;
        }   
    } 
    va_end(arg); 
    return length;
}

