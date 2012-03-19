#!/bin/bash
find . -name "*.h" -o -name "*.c" > cscope.file
cscope -bkq -i cscope.file
ctags -Rb
