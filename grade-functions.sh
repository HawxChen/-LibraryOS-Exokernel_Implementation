verbose=false

if [ "x$1" = "x-v" ]
then
	verbose=true
	out=/dev/stdout
	err=/dev/stderr
else
	out=/dev/null
	err=/dev/null
fi

if gmake --version >/dev/null 2>&1; then make=gmake; else make=make; fi

#
# QEMU
#

timeout=30
preservefs=n
qemu=`$make -s --no-print-directory print-qemu`
gdbport=`$make -s --no-print-directory print-gdbport`
qemugdb=`$make -s --no-print-directory print-qemugdb`
brkfn=readline

echo_n () {
	# suns can't echo -n, and Mac OS X can't echo "x\c"
	# assume argument has no doublequotes
	awk 'BEGIN { printf("'"$*"'"); }' </dev/null
}

# Run QEMU with serial output redirected to jos.out.  If $brkfn is
# non-empty, wait until $brkfn is reached or $timeout expires, then
# kill QEMU.
run () {
	qemuextra=
	if [ "$brkfn" ]; then
		qemuextra="-S $qemugdb"
	fi

	qemucommand="$qemu -nographic $qemuopts -serial file:jos.out -monitor null -no-reboot $qemuextra"
	if $verbose; then
		echo $qemucommand 1>&2
	fi

	t0=`date +%s.%N 2>/dev/null`
	(
		ulimit -t $timeout
		exec $qemucommand
	) >$out 2>$err &
	PID=$!

	# Wait for QEMU to start
	sleep 1

	if [ "$brkfn" ]; then
		# Find the address of the kernel $brkfn function,
		# which is typically what the kernel monitor uses to
		# read commands interactively.
		brkaddr=`grep " $brkfn\$" obj/kern/kernel.sym | sed -e's/ .*$//g'`

		(
			echo "target remote localhost:$gdbport"
			echo "br *0x$brkaddr"
			echo c
		) > jos.in
		gdb -batch -nx -x jos.in > /dev/null 2>&1

		# Make sure QEMU is dead.  On OS X, exiting gdb
		# doesn't always exit QEMU.
		kill $PID > /dev/null 2>&1
	fi
}

#
# Scoring
#

pts=5
part=0
partpos=0
total=0
totalpos=0

showpart () {
	echo "Part $1 score: $part/$partpos"
	echo
	total=`expr $total + $part`
	totalpos=`expr $totalpos + $partpos`
	part=0
	partpos=0
}

showfinal () {
	total=`expr $total + $part`
	totalpos=`expr $totalpos + $partpos`
	echo "Score: $total/$totalpos"
	if [ $total -lt $totalpos ]; then
		exit 1
	fi
}

passfailmsg () {
	msg="$1"
	shift
	if [ $# -gt 0 ]; then
		msg="$msg,"
	fi

	t1=`date +%s.%N 2>/dev/null`
	time=`echo "scale=1; ($t1-$t0)/1" | sed 's/.N/.0/g' | bc 2>/dev/null`

	echo $msg "$@" "(${time}s)"
}

pass () {
	passfailmsg OK "$@"
	part=`expr $part + $pts`
	partpos=`expr $partpos + $pts`
}

fail () {
	passfailmsg WRONG "$@"
	partpos=`expr $partpos + $pts`
	if $verbose; then
		exit 1
	fi
}


#
# User tests
#

# Usage: runtest <tagname> <defs> <check fn> <check args...>
runtest () {
	perl -e "print '$1: '"
	rm -f obj/kern/init.o obj/kern/kernel obj/kern/kernel.img 
	[ "$preservefs" = y ] || rm -f obj/fs/fs.img
	if $verbose
	then
		echo "$make $2... "
	fi
	$make $2 >$out
	if [ $? -ne 0 ]
	then
		rm -f obj/kern/init.o
		echo $make $2 failed 
		exit 1
	fi
	# We just built a weird init.o that runs a specific test.  As
	# a result, 'make qemu' will run the last graded test and
	# 'make clean; make qemu' will run the user-specified
	# environment.  Remove our weird init.o to fix this.
	rm -f obj/kern/init.o
	run

	# Give qemu some more time to run (for asynchronous mode).
	# This way, we get the small 1 second wait for most tests
	# and a longer wait (5 seconds) in case qemu needs that
	# time to load.
	if [ ! -s jos.out ]
	then
		sleep 4
	fi

	if [ ! -s jos.out ]
	then
		fail > /dev/null   # Still increment number of possible points
		echo 'no jos.out'
	else
		shift
		shift
		check=$1
		shift
		$check "$@"
	fi
}

quicktest () {
	perl -e "print '$1: '"
	shift
	checkregexps "$@"
}

checkregexps () {
	okay=yes

	not=false
	for i
	do
		if [ "x$i" = "x!" ]
		then
			not=true
		elif $not
		then
			if egrep "^$i\$" jos.out >/dev/null
			then
				echo "got unexpected line '$i'"
				okay=no
			fi
			not=false
		else
			egrep "^$i\$" jos.out >/dev/null
			if [ $? -ne 0 ]
			then
				echo "missing '$i'"
				okay=no
			fi
			not=false
		fi
	done
	if [ "$okay" = "yes" ]
	then
		pass
	else
		fail
	fi
}

# Usage: runtest1 [-tag <tagname>] [-dir <dirname>] <progname> [-Ddef...] [-check checkfn] checkargs...
runtest1 () {
	tag=
	dir=user
	check=checkregexps
	while true; do
		if [ $1 = -tag ]
		then
			tag=$2
		elif [ $1 = -dir ]
		then
			dir=$2
		else
			break
		fi
		shift
		shift
	done
	prog=$1
	shift
	if [ "x$tag" = x ]
	then
		tag=$prog
	fi
	runtest1_defs=
	while expr "x$1" : 'x-D.*' >/dev/null; do
		runtest1_defs="DEFS+='$1' $runtest1_defs"
		shift
	done
	if [ "x$1" = "x-check" ]; then
		check=$2
		shift
		shift
	fi
	runtest "$tag" "DEFS='-DTEST=${dir}_${prog}' $runtest1_defs" "$check" "$@"
}

