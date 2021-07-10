/*
 * Orbtrace Interface
 * ==================
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

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#if defined OSX
    #include <sys/ioctl.h>
    #include <libusb.h>
    #include <termios.h>
#else
    #if defined LINUX
        #include <libusb-1.0/libusb.h>
        #include <asm/ioctls.h>
        #if defined TCGETS2
            #include <asm/termios.h>
            /* Manual declaration to avoid conflict. */
            extern int ioctl ( int __fd, unsigned long int __request, ... ) __THROW;
        #else
            #include <sys/ioctl.h>
            #include <termios.h>
        #endif
    #else
        #error "Unknown OS"
    #endif
#endif
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>
#include <signal.h>

#include "git_version_info.h"
#include "generics.h"

#include "orbtraceIf.h"

/* Table of known devices to try opening */
static const struct deviceList
{
    uint32_t vid;
    uint32_t pid;
    char *name;
} _deviceList[] =
{
    { 0x1209, 0x3443, "Orbtrace"         },
    { 0, 0, 0 }
};

#define DONTSET (-1)
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

/* Record for options, either defaults or from command line */
struct
{
    char *sn;                 /* Any part of serial number to differentiate probe */
    char *qQuery;             /* V/I Parameters to query from probe */
    int brightness;           /* Brightness of OP LEDs */
    int traceWidth;           /* Width to be used for communication */
    bool forceVoltage;        /* Force application of voltage */
    bool opJSON;              /* Set output to JSON */
    bool listDevices;         /* List devices connected to system */
    int TPwrmv;               /* Target power setting in mv */
    int TRefmv;               /* Target voltage setting in mv */
    bool writeParams;         /* Write specified parameters to NVRAM */
    bool resetParams;         /* Reset all parameters in NVRAM */
    bool unlock;              /* Unlock device */
    bool lock;                /* Lock device */
} options =
{
  .traceWidth = DONTSET,
  .brightness = DONTSET,
  .TPwrmv = DONTSET,
    .TRefmv = DONTSET
};

struct
{
    /* Link to the connected Orbtrace device */
    struct OrbtraceIf *dev;

    bool      ending;                                                  /* Flag indicating app is terminating */
} _r;

// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
// Private routines
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
static void _intHandler( int sig )

{
    /* CTRL-C exit is not an error... */
    exit( 0 );
}
// ====================================================================================================
void _printHelp( char *progName )

{
    genericsPrintf( "Usage: %s [options]" EOL, progName );
    genericsPrintf( "       -b: <Brightness> Set default brightness of output leds" EOL );
    genericsPrintf( "       -f: <filename> Take input from specified file" EOL );
    genericsPrintf( "       -h: This help" EOL );
    genericsPrintf( "       -j: Format output in JSON" EOL );
    genericsPrintf( "       -l: Show all OrbTrace devices attached to system" EOL );
    genericsPrintf( "       -L: Lock device (prevent further changes)" EOL);
    genericsPrintf( "       -o: <num> Specify 1, 2 or 4 bits trace width" EOL );
    genericsPrintf( "       -q: Query all data from connected device" EOL );
    genericsPrintf( "       -Q: Query specified data from connected device (pPrR VPwr/IPwr/VRef/IRef)" EOL );
    genericsPrintf( "       -p: Set TPwr voltage (0=Off)" EOL );
    genericsPrintf( "       -r: Set TRef voltage (0=Passive)" EOL );
    genericsPrintf( "       -s: <Serial> any part of serial number to differentiate specific OrbTrace device" EOL );
    genericsPrintf( "       -U: Unlock device (allow changes, default state)" EOL );
    genericsPrintf( "       -v: <level> Verbose mode 0(errors)..3(debug)" EOL );
    genericsPrintf( "       -w: Write parameters specified on command line to NVRAM" EOL );
    genericsPrintf( "       -W: Reset all NVRAM parameters to default values" EOL );
}
// ====================================================================================================
int _processOptions( int argc, char *argv[] )

{
    int c;
    float voltage;
    int optionsSet = 0;

    while ( ( c = getopt ( argc, argv, "a:ef:hl:m:no:p:s:v:" ) ) != -1 )
        switch ( c )
        {
            // ------------------------------------
            case 'b': /* Brightness */
                options.brightness = atoi( optarg );
                optionsSet++;
                break;

            // ------------------------------------
            case 'F': /* Input filename */
              options.forceVoltage = true;
                break;

            // ------------------------------------
            case 'h':
                _printHelp( argv[0] );
                return false;

            // ------------------------------------
            case 'j': /* Force output in JSON */
                options.opJSON = true;
                break;

            // ------------------------------------
            case 'l': /* List connected devices */
                options.listDevices = true;
                break;

            // ------------------------------------
        case 'L': /* Lock device */
                options.lock = true;
                break;

            // ------------------------------------
            case 'o':
                options.traceWidth = atoi( optarg );
                optionsSet++;
                break;

            // ------------------------------------
            case 'p':
                voltage = atof( optarg );
                options.TPwrmv = ( int )( ( voltage + 0.00005F ) * 1000 );
                optionsSet++;
                break;

            // ------------------------------------
            case 'r':
                voltage = atof( optarg );
                options.TRefmv = ( int )( ( voltage + 0.00005F ) * 1000 );
                optionsSet++;
                break;

            // ------------------------------------

            case 's':
                options.sn = optarg;
                break;

            // ------------------------------------
        case 'U': /* Unlock device */
                options.unlock = true;
                break;

            // ------------------------------------
            case 'v':
                genericsSetReportLevel( atoi( optarg ) );
                break;

            // ------------------------------------
            case 'w': /* Write parameters to NVRAM */
                options.writeParams = true;
                break;
            // ------------------------------------
            case 'W': /* Reset parameters in NVRAM */
                options.resetParams = true;
                break;
            // ------------------------------------
            case '?':
                if ( optopt == 'b' )
                {
                    genericsReport( V_ERROR, "Option '%c' requires an argument." EOL, optopt );
                }
                else if ( !isprint ( optopt ) )
                {
                    genericsReport( V_ERROR, "Unknown option character `\\x%x'." EOL, optopt );
                }

                return false;

            // ------------------------------------
            default:
                genericsReport( V_ERROR, "Unrecognised option '%c'" EOL, c );
                return false;
                // ------------------------------------
        }

    /* Test parameters for sanity */
    if ( optionsSet )
    {
        if ( options.resetParams )
        {
            genericsReport( V_ERROR, "Cannot set a parameter while reseting all parameters" EOL );
            return false;
        }

        if ( options.listDevices )
        {
            genericsReport( V_ERROR, "Cannot list devices while setting a parameter" EOL );
            return false;
        }
    }

    if ( ( options.traceWidth != DONTSET ) &&
            ( options.traceWidth != 1 ) &&
            ( options.traceWidth != 2 ) &&
            ( options.traceWidth == 4 ) )
    {
        genericsReport( V_ERROR, "Orbtrace interface illegal port width" EOL );
        return false;
    }


    if ( ( options.TRefmv ) && ( !OrbtraceIfValidateVoltage( NULL, options.TRefmv ) ) )
{
    genericsReport( V_ERROR, "Illegal voltage specified for TRef (%d.%03d)V" EOL, options.TRefmv / 1000, options.TRefmv % 1000 );
        return false;
    }

    if ( ( options.TPwrmv ) && ( !OrbtraceIfValidateVoltage( NULL, options.TPwrmv ) ) )
{
    genericsReport( V_ERROR, "Illegal voltage specified for TPwr (%d.%03d)V" EOL, options.TPwrmv / 1000, options.TPwrmv % 1000 );
        return false;
    }

    if ( ( options.brightness != DONTSET ) && ( ( options.brightness < 0 ) || ( options.brightness > 255 ) ) )
{
    genericsReport( V_ERROR, "Brightness setting out of range" EOL );
        return false;
    }

    /* ... and dump the config if we're being verbose */
    genericsReport( V_INFO, "%s V" VERSION " (Git %08X %s, Built " BUILD_DATE ")" EOL, argv[0], GIT_HASH, ( GIT_DIRTY ? "Dirty" : "Clean" ) );

    return true;
}
// ====================================================================================================
static void _doExit( void )

{
    _r.ending = true;
}
// ====================================================================================================
static int _selectDevice( struct OrbtraceIf *o, int ndevices )

{
  int descWidth = 0;
  int selection = 0;
    for (int i=0; i<ndevices; i++)
      {
        int l = MAX(11,strlen(OrbtraceIfGetManufacturer(o,i))+strlen(OrbtraceIfGetProduct(o,i)))+MAX(6,strlen(OrbtraceIfGetSN(o,i)));
        if (l>descWidth) descWidth=l;
      }

    descWidth+=1;

    fprintf(stdout,"Id | ");
    for (int i=0; i<(descWidth/2-6); i++) fprintf(stdout," ");
    fprintf(stdout,"Description");
    for (int i=0; i<(descWidth/2-6); i++) fprintf(stdout," ");
    fprintf(stdout," | Serial" EOL);
    for (int i=0; i<(descWidth+5+10); i++) fprintf(stdout,"-");
    fprintf(stdout,EOL);

    for (int i=0; i<ndevices; i++)
      {
        int thisWidth = strlen(OrbtraceIfGetManufacturer(o,i))+strlen(OrbtraceIfGetProduct(o,i));
        printf("%2i | %s %s",i+1,OrbtraceIfGetManufacturer(o,i),OrbtraceIfGetProduct(o,i));
        for (int j=thisWidth+2; j<descWidth; j++) fprintf(stdout," ");
        fprintf(stdout,"| %s" EOL,OrbtraceIfGetSN(o,i));
      }

    while ((selection<1) || (selection>ndevices))
      {
        fprintf(stdout,"Selection>");
        scanf("%d",&selection);
      }

    return selection-1;
}

// ====================================================================================================
int main( int argc, char *argv[] )

{
  int selection = 0;
    if ( !_processOptions( argc, argv ) )
    {
        /* processOptions generates its own error messages */
        genericsExit( -1, "" EOL );
    }

    /* Make sure everything gets removed at the end */
    atexit( _doExit );

    /* This ensures the atexit gets called */
    if ( SIG_ERR == signal( SIGINT, _intHandler ) )
    {
        genericsExit( -1, "Failed to establish Int handler" EOL );
    }

    struct OrbtraceIf *o=OrbtraceIfCreateContext();
    assert(o);

    int ndevices = OrbtraceIfGetDeviceList( o, options.sn );

    /* Allow option to choose between devices if there's more than one found */
    if (ndevices>1)
      selection = _selectDevice( o, ndevices );

    genericsReport( V_INFO, "Got device [%s %s, S/N %s]" EOL,
                    OrbtraceIfGetManufacturer(o,selection),
                    OrbtraceIfGetProduct(o,selection),
                    OrbtraceIfGetSN(o,selection));
    if (!OrbtraceIfOpenDevice( o, selection ))
        genericsExit( -1, "Couldn't open device" EOL );
}
// ====================================================================================================
