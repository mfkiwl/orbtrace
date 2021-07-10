/*
 * Generic Routines
 * ================
 *
 * Copyright (C) 2017, 2019  Dave Marples  <dave@marples.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the names Orbtrace, Orbuculum nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _GENERICS_
#define _GENERICS_
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>
#include <errno.h>

#if defined LINUX
    #define EOL "\n"
#else
    #define EOL "\n\r"
#endif
#ifdef __cplusplus
extern "C" {
#endif

/* Error return codes */
#define OK         0
#define ERR       -1

#ifdef SCREEN_HANDLING
#define CLEAR_SCREEN  "\033[2J\033[;H"
#define C_PREV_LN     "\033[1F"
#define C_CLR_LN      "\033[K"
#else
#define CLEAR_SCREEN  ""
#define C_PREV_LN ""
#define C_CLR_LN  ""
#endif

#define C_RES     "\033[0m"
#define C_RED     "\033[0;31m"
#define C_GREEN   "\033[0;32m"
#define C_BROWN   "\033[0;33m"
#define C_BLUE    "\033[0;34m"
#define C_PURPLE  "\033[0;35m"
#define C_CYAN    "\033[0;36m"
#define C_GRAY    "\033[0;37m"
#define C_LRED    "\033[1;31m"
#define C_LGREEN  "\033[1;32m"
#define C_YELLOW  "\033[1;33m"
#define C_LBLUE   "\033[1;34m"
#define C_LPURPLE "\033[1;35m"
#define C_LCYAN   "\033[1;36m"
#define C_WHITE   "\033[1;37m"
#define C_MONO    ""

// ====================================================================================================
enum verbLevel {V_ERROR, V_WARN, V_INFO, V_DEBUG, V_MAX_VERBLEVEL};


char *genericsEscape( char *str );
char *genericsUnescape( char *str );
uint64_t genericsTimestampuS( void );
uint32_t genericsTimestampmS( void );
void genericsSetReportLevel( enum verbLevel lset );
enum verbLevel genericsGetReportLevel( void );
void genericsPrintf( const char *fmt, ... );
void genericsReport( enum verbLevel l, const char *fmt, ... );
void genericsExit( int status, const char *fmt, ... );
// ====================================================================================================
#ifdef __cplusplus
}
#endif

#endif
