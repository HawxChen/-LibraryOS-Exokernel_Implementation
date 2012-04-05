#!/bin/tcsh
./upcode_util/cscope.sh
./upcode_util/indent.sh
find . -name '\.*\.sw*' | xargs rm
