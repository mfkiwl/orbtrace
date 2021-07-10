/*
 * Orbtrace Interface Module
 * =========================
 *
 * Copyright (C) 2020 Dave Marples  <dave@marples.net>
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "orbtraceIf.h"

/* List of device VID/PID pairs this library works with */
static const struct
{
  int vid;
  int pid;
} _validDevices[] =
  {
    { 0x1209, 0x3443 },
    { 0x046d, 0x0892 },
    { 0     , 0 }
  };

/* Maximum descriptor length from USB specification */
#define MAX_USB_DESC_LEN (256)

// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
static void _flushDeviceList( struct OrbtraceIf *o )

/* Flush out any old device records */

{
  for (size_t i=0; i<o->numDevices; i++)
    {
      if (o->device[i].sn) free(o->device[i].sn);
      if (o->device[i].manufacturer) free(o->device[i].manufacturer);
      if (o->device[i].product) free(o->device[i].product);
    }
  o->numDevices=0;
  o->device=NULL;
}
// ====================================================================================================
static int _strcmpint( char *s1, char *s2 )

/* Version of strcmp that accomodates NULLs */

{
  if ((s1) && (!s2)) return -1;
  if ((!s1) && (s2)) return 1;

  return strcmp(s1,s2);
}
// ====================================================================================================
static int _compareFunc( const void *vd1, const void *vd2 )

/* Return a comparison for two devices, used for qsort ordering */

{
  const struct OrbtraceIfDevice *d1=(const struct OrbtraceIfDevice *)vd1;
  const struct OrbtraceIfDevice *d2=(const struct OrbtraceIfDevice *)vd2;
  int r=0;

  if ((r=_strcmpint(d1->manufacturer,d2->manufacturer))) return r;
  if ((r=_strcmpint(d1->product,d2->product))) return r;
  if ((r=strcmp(d1->sn,d2->sn))) return r;
  if ((r=d1->vid-d2->vid)) return r;
  return d1->pid-d2->pid;
}
// ====================================================================================================
// ====================================================================================================
// ====================================================================================================
bool OrbtraceIfValidateVoltage( struct OrbtraceIf *o, int vmv )

{
  return true;
}
// ====================================================================================================
struct OrbtraceIf *OrbtraceIfCreateContext( void )

{
  struct OrbtraceIf *o = (struct OrbtraceIf *)calloc(1,sizeof(struct OrbtraceIf));

  if (libusb_init( &o->context ) < 0)
    {
      free( o );
      return NULL;
    }
  else
    {
      return o;
    }
}

// ====================================================================================================
void OrbtraceIfDestroyContext( struct OrbtraceIf *o )

{
  assert(o);
  /* Flush out any old scans we might be holding */
  _flushDeviceList(o);
  if (o->list) libusb_free_device_list(o->list,1);
  free(o);
}
// ====================================================================================================
int OrbtraceIfGetDeviceList( struct OrbtraceIf *o, char *sn )

/* Get list of devices that match (partial) serial number, VID and PID */

{
  size_t y;
  char tfrString[MAX_USB_DESC_LEN];
  libusb_device_handle *h;
  struct OrbtraceIfDevice d;

  /* Flush out any old scans we might be holding */
  _flushDeviceList(o);
  if (o->list) libusb_free_device_list(o->list,1);

  int count = libusb_get_device_list(o->context, &o->list);

  for (size_t i = 0; i < count; i++)
    {
      libusb_device *device = o->list[i];
      struct libusb_device_descriptor desc = { 0 };
      libusb_get_device_descriptor(device, &desc);

      /* Loop through known devices to see if this one is one we recognise */
      for (y=0; ((_validDevices[y].vid) &&
                 ((_validDevices[y].vid!=desc.idVendor) || (_validDevices[y].pid!=desc.idProduct))); y++);

      /* If it's one we're interested in then process further */
      if (_validDevices[y].vid)
        {
          memset(&d,0,sizeof(d));
          if (libusb_open( o->list[i], &h ) >=0 )
            {
              if (desc.iSerialNumber)
                libusb_get_string_descriptor_ascii( h, desc.iSerialNumber, (unsigned char *)tfrString, MAX_USB_DESC_LEN );

              /* This is a match if no S/N match was requested or if there is a S/N and they part-match */
              if ((!sn) || ((desc.iSerialNumber) && (strstr(tfrString,sn))))
                {
                  d.sn = strdup(tfrString);

                  if (desc.iManufacturer)
                    {
                      libusb_get_string_descriptor_ascii( h, desc.iManufacturer, (unsigned char *)tfrString, MAX_USB_DESC_LEN );
                      d.manufacturer = strdup(tfrString);
                    }
                  if (desc.iProduct)
                    {
                      libusb_get_string_descriptor_ascii( h, desc.iProduct, (unsigned char *)tfrString, MAX_USB_DESC_LEN );
                      d.product = strdup(tfrString);
                    }

                  d.devIndex = i;

                  /* Now store this match for access later */
                  o->device = realloc( o->device, o->numDevices+1 );
                  memcpy(&o->device[o->numDevices], &d, sizeof(struct OrbtraceIfDevice));
                  o->numDevices++;
                }

              libusb_close( h );
            }
        }
    }

  /* Now sort matching devices into defined order, so they're always the same way up */
  qsort( o->device, o->numDevices, sizeof(struct OrbtraceIfDevice), _compareFunc );

  return o->numDevices;
}
// ====================================================================================================
bool OrbtraceIfOpenDevice( struct OrbtraceIf *o, int entry )

{
  return false;
}
// ====================================================================================================
