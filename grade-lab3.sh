#!/bin/sh

qemuopts="-hda obj/kern/kernel.img"
. ./grade-functions.sh


$make

# the [00001000] tags should have [] in them, but that's 
# a regular expression reserved character, and i'll be damned if
# I can figure out how many \ i need to add to get through 
# however many times the shell interprets this string.  sigh.

pts=10

runtest1 divzero \
	! '1/0 is ........!' \
	'Incoming TRAP frame at 0xefbfff..' \
	'TRAP frame at 0xf.......' \
	'  trap 0x00000000 Divide error' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00001000. free env 00001000'

runtest1 softint \
	'Welcome to the JOS kernel monitor!' \
	'Incoming TRAP frame at 0xefbfffbc' \
	'TRAP frame at 0xf.......' \
	'  trap 0x0000000d General Protection' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00001000. free env 00001000'

runtest1 badsegment \
	'Incoming TRAP frame at 0xefbfffbc' \
	'TRAP frame at 0xf.......' \
	'  trap 0x0000000d General Protection' \
	'  err  0x00000028' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	'.00001000. free env 00001000'

showpart A

pts=5

runtest1 faultread \
	! 'I read ........ from location 0!' \
	'.00001000. user fault va 00000000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
	'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000004.*' \
	'.00001000. free env 00001000'

runtest1 faultreadkernel \
	! 'I read ........ from location 0xf0100000!' \
	'.00001000. user fault va f0100000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
	'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000005.*' \
	'.00001000. free env 00001000' \

runtest1 faultwrite \
	'.00001000. user fault va 00000000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
	'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000006.*' \
	'.00001000. free env 00001000'

runtest1 faultwritekernel \
	'.00001000. user fault va f0100000 ip 008.....' \
	'Incoming TRAP frame at 0xefbfffbc' \
	'TRAP frame at 0xf.......' \
	'  trap 0x0000000e Page Fault' \
	'  err  0x00000007.*' \
	'.00001000. free env 00001000'


runtest1 breakpoint \
	'Welcome to the JOS kernel monitor!' \
	'Incoming TRAP frame at 0xefbfffbc' \
	'TRAP frame at 0xf.......' \
	'  trap 0x00000003 Breakpoint' \
	'  eip  0x008.....' \
	'  ss   0x----0023' \
	! '.00001000. free env 00001000'

runtest1 testbss \
	'Making sure bss works right...' \
	'Yes, good.  Now doing a wild write off the end...' \
	'.00001000. user fault va 00c..... ip 008.....' \
	'.00001000. free env 00001000'

runtest1 hello \
	'.00000000. new env 00001000' \
	'hello, world' \
	'i am environment 00001000' \
	'.00001000. exiting gracefully' \
	'.00001000. free env 00001000' \
	'Destroyed the only environment - nothing more to do!'

runtest1 buggyhello \
	'.00001000. user_mem_check assertion failure for va 00000001' \
	'.00001000. free env 00001000'

runtest1 buggyhello2 \
	'.00001000. user_mem_check assertion failure for va 0....000' \
	'.00001000. free env 00001000' \
	! 'hello, world'

runtest1 evilhello \
	'.00001000. user_mem_check assertion failure for va f0100...' \
	'.00001000. free env 00001000'

showpart B

showfinal
