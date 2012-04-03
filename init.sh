#!/bin/tcsh
./upcode_util/cscope.sh
./upcode_util/indent.sh
find . -name '\.*\.swp' | xargs rm
