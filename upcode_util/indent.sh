#!/bin/bash
cd ..
find . -name '*.c'| xargs indent -nbad -bap -nbc -bbo -bl -bli0 -bls -ncdb -ncdw -nce -cp1 -cs -di1 -ndj -nfc1 -nfca -hnl -i4 -ip5 -lp -pcs -nprs -psl -saf -sai -saw -nsc -nsob -nut -ts4
find . -name '*.h'| xargs indent -nbad -bap -nbc -bbo -bl -bli0 -bls -ncdb -ncdw -nce -cp1 -cs -di1 -ndj -nfc1 -nfca -hnl -i4 -ip5 -lp -pcs -nprs -psl -saf -sai -saw -nsc -nsob -nut -ts4
find . -name '*.h~' | xargs rm
find . -name '*.c~' | xargs rm
