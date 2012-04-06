#!/bin/sh

qemuopts="-hda obj/kern/kernel.img"
. ./grade-functions.sh


$make

timeout=10
E0='07'
E1='08'
E2='09'
E3='0a'
E4='0b'
E5='0c'
E6='0d'
E7='0e'

runtest1 dumbfork \
	".00000000. new env 00001000" \
	".00000000. new env 000010$E1" \
	"0: I am the parent." \
	"9: I am the parent." \
	"0: I am the child." \
	"9: I am the child." \
	"19: I am the child." \
	".000010$E1. exiting gracefully" \
	".000010$E1. free env 000010$E1" \
	".000010$E2. exiting gracefully" \
	".000010$E2. free env 000010$E2"

showpart A

runtest1 faultread \
	! "I read ........ from location 0." \
	".000010$E1. user fault va 00000000 ip 008....." \
	"TRAP frame at 0xf....... from CPU ." \
	"  trap 0x0000000e Page Fault" \
	"  err  0x00000004.*" \
	".000010$E1. free env 000010$E1"

runtest1 faultwrite \
	".000010$E1. user fault va 00000000 ip 008....." \
	"TRAP frame at 0xf....... from CPU ." \
	"  trap 0x0000000e Page Fault" \
	"  err  0x00000006.*" \
	".000010$E1. free env 000010$E1"

runtest1 faultdie \
	"i faulted at va deadbeef, err 6" \
	".000010$E1. exiting gracefully" \
	".000010$E1. free env 000010$E1" 

runtest1 faultregs \
	"Registers in UTrapframe OK" \
	! "Registers in UTrapframe MISMATCH" \
	"Registers after page-fault OK" \
	! "Registers after page-fault MISMATCH"

runtest1 faultalloc \
	"fault deadbeef" \
	"this string was faulted in at deadbeef" \
	"fault cafebffe" \
	"fault cafec000" \
	"this string was faulted in at cafebffe" \
	".000010$E1. exiting gracefully" \
	".000010$E1. free env 000010$E1"

runtest1 faultallocbad \
	".000010$E1. user_mem_check assertion failure for va deadbeef" \
	".000010$E1. free env 000010$E1" 

runtest1 faultnostack \
	".000010$E1. user_mem_check assertion failure for va eebfff.." \
	".000010$E1. free env 000010$E1"

runtest1 faultbadhandler \
	".000010$E1. user_mem_check assertion failure for va (deadb|eebfe)..." \
	".000010$E1. free env 000010$E1"

runtest1 faultevilhandler \
	".000010$E1. user_mem_check assertion failure for va (f0100|eebfe)..." \
	".000010$E1. free env 000010$E1"

runtest1 forktree \
	"....: I am .0." \
	"....: I am .1." \
	"....: I am .000." \
	"....: I am .100." \
	"....: I am .110." \
	"....: I am .111." \
	"....: I am .011." \
	"....: I am .001." \
	".000010$E1. exiting gracefully" \
	".000010$E2. exiting gracefully" \
	".0000200.. exiting gracefully" \
	".0000200.. free env 0000200."

showpart B

runtest1 spin \
	".00000000. new env 00001000" \
	".00000000. new env 000010$E1" \
	"I am the parent.  Forking the child..." \
	".000010$E1. new env 000010$E2" \
	"I am the parent.  Running the child..." \
	"I am the child.  Spinning..." \
	"I am the parent.  Killing the child..." \
	".000010$E1. destroying 000010$E2" \
	".000010$E1. free env 000010$E2" \
	".000010$E1. exiting gracefully" \
	".000010$E1. free env 000010$E1"

qemuopts="$qemuopts -smp 2"

runtest1 stresssched \
	".000010... stresssched on CPU 0" \
	".000010... stresssched on CPU 1" \
	! ".*ran on two CPUs at once"

runtest1 pingpong \
	".00000000. new env 00001000" \
	".00000000. new env 000010$E1" \
	".000010$E1. new env 000010$E2" \
	"send 0 from 10$E1 to 10$E2" \
	"10$E2 got 0 from 10$E1" \
	"10$E1 got 1 from 10$E2" \
	"10$E2 got 8 from 10$E1" \
	"10$E1 got 9 from 10$E2" \
	"10$E2 got 10 from 10$E1" \
	".000010$E1. exiting gracefully" \
	".000010$E1. free env 000010$E1" \
	".000010$E2. exiting gracefully" \
	".000010$E2. free env 000010$E2" \

timeout=30
runtest1 primes \
	".00000000. new env 00001000" \
	".00000000. new env 000010$E1" \
	".000010$E1. new env 000010$E2" \
	"CPU .: 2 .000010$E2. new env 000010$E3" \
	"CPU .: 3 .000010$E3. new env 000010$E4" \
	"CPU .: 5 .000010$E4. new env 000010$E5" \
	"CPU .: 7 .000010$E5. new env 000010$E6" \
	"CPU .: 11 .000010$E6. new env 000010$E7" \
	"CPU .: 1877 .00001128. new env 00001129"

showpart C

showfinal
