#!/bin/bash
find . -name "*.h" -o -name "*.c" -o -name "*.S" > cscope.file
cscope -bkq -i cscope.file
ctags -Rb
