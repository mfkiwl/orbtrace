import usb.backend.libusb1

VENDOR_ID = 0x1209
PRODUCT_ID = 0x3443
INTERFACE = 0
IN_EP = (1|0x80)
OUT_EP = 1

def write_to_usb(dev, msg_str):

    print(">>>",end="")
    for p in msg_str:
        print(f' 0x{p:02x}', end="")

    try:
        num_bytes_written = dev.write(OUT_EP, msg_str)

    except usb.core.USBError as e:
        print (e.args)

    print(f" [{num_bytes_written}]")
    return num_bytes_written

def read_from_usb(dev, rxlen, timeout):
    try:
	# try to read a maximum of 2^16-1 bytes from 0x81 (IN endpoint)
        data = dev.read(IN_EP, 60000, timeout)
    except usb.core.USBError as e:
        print ("Error reading response: {}".format(e.args))
        exit(-1)

    if len(data) == 0:
        print ("Zero length response")
        exit(-1)

    return data

def op_response( d, compareStr ):
    duff=False
    print(" <<<"+str(len(d))+f" bytes [",end="")
    for p,x in zip(d,compareStr):
        if p==x:
            print(f' 0x{p:02x}', end="")
        else:
            duff=True
            print(f' Got:0x{p:02x}/0x{x:02x}!!', end="")
    if (duff):
        print(" ] *********************** FAULT **********************")
    else:
        print(" ]")


tests2 = (
    ( "Short Request",              b"\x19\x19",                    b"\xff"                     ),
    ( "Vendor ID",                  b"\x00\x01",                    b"\x00\x00"                 ),
    ( "Product ID",                 b"\x00\x02",                    b"\x00\x00"                 ),
    ( "Serial Number",              b"\x00\x03",                    b"\x00\x00"                 ),
    ( "Target Device Vendor",       b"\x00\x05",                    b"\x00\x00"                 ),
    ( "Target Device Name",         b"\x00\x06",                    b"\x00\x00"                 ),
    ( "FW version",                 b"\x00\x04",                    b"\x00\x05\x31\x2e\x30\x30\x00" ),
    ( "Illegal command",            b"\x42",                        b"\xff"                     ),
    ( "Request CAPABILITIES",       b"\x00\xf0",                    b"\x00\x01\x01"             ),
    ( "Request TEST DOMAIN TIMER",  b"\x00\xf1",                    b"\x00\x08\x00\xca\x9a\x3b" ),
    ( "Request SWO Trace Buffer Size", b"\x00\xfd",                 b"\x00\x04\xe8\x03\x00\x00" ),
    ( "Request Packet Count",       b"\x00\xFE",                    b"\x00\x01\x40"             ),
    ( "Request Packet Size",        b"\x00\xff",                    b"\x00\x02\x40\x00"         ),
    ( "Set connect led",            b"\x01\x00\x01",                b"\x01\x00"                 ),
    ( "Set running led",            b"\x01\x01\x01",                b"\x01\x00"                 ),
    ( "Set illegal led",            b"\x01\x02\x01",                b"\xff"                     ),
    ( "Connect swd",                b"\x02\x01",                    b"\x02\x01"                 ),
    ( "Connect default",            b"\x02\x00",                    b"\x02\x01"                 ),
    ( "Connect JTAG",               b"\x02\x02",                    b"\xff"                     ),
    ( "Disconnect",                 b"\x03",                        b"\x03\x00"                 ),
    ( "WriteABORT",                 b"\x08\x00\x01\x02\x03\x04",    b"\x08\x00"                 ),
    ( "Delay",                      b"\x09\x01\x02\x03\x04",        b"\x09\x00"                 ),
    ( "ResetTarget",                b"\x0A" ,                       b"\x0A\x00\x01"             ),
    ( "DAP_SWJ_Pins",               b"\x10\x17\x17\x00\01\x02\x03", b"\x10\x99"                 ),
    ( "DAP_SWJ_Clock",              b"\x11\x00\x01\x02\x03",        b"\x11\x00"                 ),
    ( "DAP_SWJ_Sequence",           b"\x12\x03\x01",    b"\x12\x00"                 ),
    ( "DAP_SWJ_Sequence (Long)",    b"\x12\x00\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04\x01\x02\x03\x04",    b"\x12\x00"                 ),
    ( "DAP_SWO_Transport (None)",   b"\x17\x00",                    b"\x17\x00"                 ),
    ( "DAP_SWO_Transport (Cmd)",    b"\x17\x01",                    b"\x17\x00"                 ),
    ( "DAP_SWO_Transport (EP)",     b"\x17\x02",                    b"\x17\x00"                 ),
    ( "DAP_SWO_Transport (Bad)",    b"\x17\x03",                    b"\x17\xff"                 ),
    ( "DAP_SWO_Mode (Off)",         b"\x18\x00",                    b"\x18\x00"                 ),
    ( "DAP_SWO_Mode (Uart)",        b"\x18\x01",                    b"\x18\x00"                 ),
    ( "DAP_SWO_Mode (Manch)",       b"\x18\x02",                    b"\x18\x00"                 ),
    ( "DAP_SWO_Mode (Bad)",         b"\x18\x03",                    b"\x18\xff"                 ),
    ( "DAP_SWO_Baudrate",           b"\x19\x01\x02\x03\x04",        b"\x19\x01\x02\x03\x04"     ),
    ( "DAP_SWO_Control (Start)",    b"\x1a\x01",                    b"\x1a\x00"                 ),
    ( "DAP_SWO_Control (Stop)",     b"\x1a\x00",                    b"\x1a\x00"                 ),
    ( "DAP_SWO_Control (Bad)",      b"\x1a\x02",                    b"\x1a\xff"                 ),
    ( "DAP_SWO_Status",             b"\x1b",                        b"\x1b\x00\x44\x33\x22\x11" ),
    ( "DAP_SWO_ExtendedStatus",     b"\x1e\x07",                    b"\x1e\x00\x44\x33\x22\x11\x88\x77\x66\x55\xcc\xbb\xaa\x99" ),
    ( "DAP_SWO_ExtendedStatus (Bad)", b"\x1e\x08",                  b"\xff"                     ),
    ( "DAP_SWO_Data (Short)",       b"\x1c\x04\x00",                b"\x1c\x00\x04\x00\x2a\x2a\x2a\x2a" ),
    ( "DAP_SWO_Data (Long)",        b"\x1c\x63\x00",                b"\x1c\x00\x63\x00\x2a\x2a\x2a\x2a" ),
    ( "DAP_SWO_Data (Too Long)",    b"\x1c\x65\x00",                b"\x1c\x00\x64\x00\x2a\x2a\x2a\x2a" ),

    ( "DAP_JTAG_Sequence (Simple)", b"\x14\x01\x08\x01",            b"\x14\x00" ),
    ( "DAP_JTAG_Sequence (W/TDO-R)",b"\x14\x01\x88\x91",        b"\x14\x00\x80" ),
    ( "DAP_JTAG_Sequence (W/2TDO)", b"\x14\x02\x88\x91\x88\x13",    b"\x14\x00\x80\x80" ),
    ( "DAP_JTAG_Sequence (W/2TDO-R)",b"\x14\x02\x87\x91\x83\x13",   b"\x14\x00\x40\x04" ),
    ( "DAP_JTAG_Sequence (W/TDO2-R)",b"\x14\x02\x8a\x91\x92",       b"\x14\x00\x80\x02" ),
    ( "DAP_JTAG_Sequence (W/2TDO2-R)",b"\x14\x02\x8f\x91\x92\x88\x13",b"\x14\x00\x80\x40\x80" ),
    ( "DAP_JTAG_Sequence (Long)",   b"\x14\x01\x80\x00\x00\x00\x00\x00\x00\x00\x00",b"\x14\x00\x80\x80\x80\x80\x80\x80\x80\x80" ),
    ( "DAP_JTAG_Sequence (Long)",   b"\x14\x01\xbf\x00\x00\x00\x00\x00\x00\x00\x00",b"\x14\x00\x80\x80\x80\x80\x80\x80\x80\x40" ),
    ( "DAP_JTAG_Configure",         b"\x15\x03\x10",                b"\x15\x00" ),
    ( "DAP_JTAG_IDCODE",            b"\x16\x00",                    b"\x16\x00\x11\x22\x33\x44" ),
    ( "DAP_TransferConfigure",      b"\x04\x03\x11\x22\x33\x44",    b"\x04\x00" ),
#    ( "DAP_TransferConfigure (Bad)",b"\x04\x03\x11\x22\x33",        b"\xff" ),
    ( "DAP_Transfer (Simple)",      b"\x05\x00\x02\xff\x11\x22\x33\x44\x55\x66\x77\x88", b"\x05\x00\x02\x11\x22\x33\x44\x55\x66\x77\x88" ),

)

tests = (
    ( "FW version",                 b"\x00\x04",                    b"\x00\x05\x31\x2e\x30\x30\x00" ),
    ( "DAP_SWJ_Clock",              b"\x11\x0f\x42\x40\x00",        b"\x11\x00"                 ),
    ( "DAP_JTAG_Sequence",          b"\x14\x07\x48\x00\x01\x00\x41\x00\x02\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x42\x00\x01\x00",
                                    b"\x14\x00\x77\x04\xa0\x4b\x41\x90\x41\x06\x00\x00\x00\x00\x00\x00\x00" )
    )


device = usb.core.find(idVendor=VENDOR_ID, idProduct=PRODUCT_ID)

if device is None:
    raise ValueError('Device not found. Please ensure it is connected')
    sys.exit(1)

# Claim interface 1 - this interface provides cmsis-dap v2 IN and OUT endpoints to write to and read from
u=usb.util.claim_interface(device, INTERFACE)
print("Interface claimed")
#read_from_usb(device,1000,10)

for desc,inseq,outsq in tests:
    print("==============",desc)
    write_to_usb(device,bytes(inseq))
    r=read_from_usb(device,len(outsq),1000)
    op_response(r,bytes(outsq))
