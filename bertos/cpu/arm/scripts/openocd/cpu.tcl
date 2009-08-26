#use combined on interfaces or targets that can't set TRST/SRST separately
reset_config srst_only srst_pulls_trst

if { [info exists CHIPNAME] } {	
   set  _CHIPNAME $CHIPNAME    
} else {	 
   set  _CHIPNAME sam7
}

if { [info exists ENDIAN] } {	
   set  _ENDIAN $ENDIAN    
} else {	 
   set  _ENDIAN little
}

if { [info exists CPUTAPID ] } {
   set _CPUTAPID $CPUTAPID
} else {
   set _CPUTAPID 0x3f0f0f0f
}

jtag newtap $_CHIPNAME cpu -irlen 4 -ircapture 0x1 -irmask 0xf -expected-id $_CPUTAPID

set _TARGETNAME [format "%s.cpu" $_CHIPNAME]
target create $_TARGETNAME arm7tdmi -endian $_ENDIAN -chain-position $_TARGETNAME -variant arm7tdmi

$_TARGETNAME configure -event reset-start [
	halt
	sleep 10

	arm7_9 dcc_downloads enable

	armv4_5 core_state arm
	arm7_9 fast_memory_access enable

	# Init - taken from the script openocd_at91sam7_ecr.script
	mww 0xfffffd44 0x00008000  # disable watchdog
	mww 0xfffffd08 0xa5000001  # enable user reset
	mww 0xfffffc20 0x00000601  # CKGR_MOR : enable the main oscillator
	sleep 10
	mww 0xfffffc2c 0x00481c0e   # CKGR_PLLR: 96.1097 MHz
	sleep 10
	mww 0xfffffc30 0x00000007  # PMC_MCKR : MCK = PLL / 2 ~= 48 MHz
	sleep 10
	mww 0xffffff60 0x003c0100  # MC_FMR: flash mode (FWS=1,FMCN=60)
	# arm7_9 force_hw_bkpts enable  # program resides in flash
	sleep 10

	poll
	flash probe 0
	flash write_image IMAGE_FILE 0x0100000 bin
]

#"script at91sam7_write_to_flash.script"

$_TARGETNAME configure -event reset-end "shutdown"

$_TARGETNAME configure -work-area-virt 0 -work-area-phys 0x00200000 -work-area-size 0x4000 -work-area-backup 0

#flash bank <driver> <base> <size> <chip_width> <bus_width>
flash bank at91sam7 0 0 0 0 0
