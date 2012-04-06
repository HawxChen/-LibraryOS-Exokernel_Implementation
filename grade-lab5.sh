#!/bin/sh

qemuopts="-hda obj/kern/kernel.img -hdb obj/fs/fs.img"
. ./grade-functions.sh


$make

qemuopts_orig="$qemuopts"
qemuopts="$qemuopts -snapshot"

runtest1 -tag 'fs i/o [fs]' hello \
	'FS can do I/O' \
	! 'idle loop can do I/O' \

quicktest 'check_bc [fs]' \
	'block cache is good' \

quicktest 'check_super [fs]' \
	'superblock is good' \

quicktest 'check_bitmap [fs]' \
	'bitmap is good' \

quicktest 'alloc_block [fs]' \
	'alloc_block is good' \

quicktest 'file_open [fs]' \
	'file_open is good' \

quicktest 'file_get_block [fs]' \
	'file_get_block is good' \

quicktest 'file_flush/file_truncate/file rewrite [fs]' \
	'file_flush is good' \
	'file_truncate is good' \
	'file rewrite is good' \

runtest1 -tag 'lib/file.c [testfile]' testfile \
	'serve_open is good' \
	'file_stat is good' \
	'file_close is good' \
	'stale fileid is good' \

quicktest 'file_read [testfile]' \
	'file_read is good'

quicktest 'file_write [testfile]' \
	'file_write is good'

quicktest 'file_read after file_write [testfile]' \
	'file_read after file_write is good'

quicktest 'open [testfile]' \
	'open is good' \

quicktest 'large file [testfile]' \
	'large file is good'

qemuopts="$qemuopts_orig"

pts=10
runtest1 -tag 'motd display [writemotd]' writemotd \
	'OLD MOTD' \
	'This is /motd, the message of the day.' \
	'NEW MOTD' \
	'This is the NEW message of the day!' \

preservefs=y
runtest1 -tag 'motd change [writemotd]' writemotd \
	'OLD MOTD' \
	'This is the NEW message of the day!' \
	'NEW MOTD' \
	! 'This is /motd, the message of the day.' \

pts=15
preservefs=n
runtest1 -tag 'spawn via icode [icode]' icode \
	'icode: read /motd' \
	'This is /motd, the message of the day.' \
	'icode: spawn /init' \
	'init: running' \
	'init: data seems okay' \
	'icode: exiting' \
	'init: bss seems okay' \
	"init: args: 'init' 'initarg1' 'initarg2'" \
	'init: exiting' \

showfinal
