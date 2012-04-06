#!/bin/sh

qemuopts="-hda obj/kern/kernel.img"
. ./grade-functions.sh


$make

check () {
	pts=20
	echo_n "Physical page allocator: "
	if grep "check_page_alloc() succeeded!" jos.out >/dev/null
	then
		pass
	else
		fail
	fi

	pts=20
	echo_n "Page management: "
	if grep "check_page() succeeded!" jos.out >/dev/null
	then
		pass
	else
		fail
	fi

	pts=20
	echo_n "Kernel page directory: "
	if grep "check_kern_pgdir() succeeded!" jos.out >/dev/null
	then
		pass
	else
		fail
	fi

	pts=10
	echo_n "Page management 2: "
	if grep "check_page_installed_pgdir() succeeded!" jos.out >/dev/null
	then
		pass
	else
		fail
	fi
}

run
check

showfinal
