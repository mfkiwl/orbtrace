/*
 * Orbtrace Interface Module
 * =========================
 *
 * Copyright (C) 2021  Dave Marples  <dave@marples.net>
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

#ifndef _ORBTRACE_IF_
#define _ORBTRACE_IF_

#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

#if defined OSX
    #include <sys/ioctl.h>
    #include <libusb.h>
    #include <termios.h>
#else
    #if defined LINUX
        #include <libusb-1.0/libusb.h>
    #else
        #error "Unknown OS"
    #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct OrbtraceInterfaceType
{
    int vid;
    int pid;
    int *voltageListmv;
};

struct OrbtraceIfDevice
{
    char *sn;
    char *manufacturer;
    char *product;
    int vid;
    int pid;
    size_t devIndex;
    const struct OrbtraceInterfaceType *type;
};

struct OrbtraceIf
{
    libusb_device_handle *handle;
    libusb_device *dev;
    libusb_device **list;
    libusb_context *context;

    const struct OrbtraceInterfaceType *type;    /* Type of interface currently connected */
    int numDevices;                              /* Number of matching devices found */
    struct OrbtraceIfDevice *device;             /* List of matching devices found */
};
// ====================================================================================================

int OrbtraceIfValidateVoltage( struct OrbtraceIf *o, int vmv );

/* Device access */
inline char *OrbtraceIfGetManufacturer( struct OrbtraceIf *o, int e )
{
    return ( ( e < o->numDevices ) && ( o->device[e].manufacturer ) ) ? o->device[e].manufacturer : "";
}
inline char *OrbtraceIfGetProduct( struct OrbtraceIf *o, int e )
{
    return ( ( e < o->numDevices ) && ( o->device[e].product ) ) ? o->device[e].product : "";
}
inline char *OrbtraceIfGetSN( struct OrbtraceIf *o, int e )
{
    return ( ( e < o->numDevices ) && ( o->device[e].sn ) ) ? o->device[e].sn : "";
}

/* Device selection management */
int OrbtraceIfGetDeviceList( struct OrbtraceIf *o, char *sn );
bool OrbtraceIfOpenDevice( struct OrbtraceIf *o, unsigned int entry );
void OrbtraceIfCloseDevice( struct OrbtraceIf *o );

/* Device context control */
struct OrbtraceIf *OrbtraceIfCreateContext( void );
void OrbtraceIfDestroyContext( struct OrbtraceIf *o );

// ====================================================================================================
#ifdef __cplusplus
}
#endif
#endif
