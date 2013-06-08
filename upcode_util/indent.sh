#!/bin/bash
#find . -name '*.c'| xargs indent -nbad -bap -nbc -bbo -bl -bli0 -bls -ncdb -ncdw -nce -cp1 -cs -di1 -ndj -nfc1 -nfca -hnl -i4 -ip5 -lp -pcs -nprs -psl -saf -sai -saw -nsc -nsob -nut -ts4
#find . -name '*.h'| xargs indent -nbad -bap -nbc -bbo -bl -bli0 -bls -ncdb -ncdw -nce -cp1 -cs -di1 -ndj -nfc1 -nfca -hnl -i4 -ip5 -lp -pcs -nprs -psl -saf -sai -saw -nsc -nsob -nut -ts4
find . -name '*.c'| xargs indent -kr -brf -bli0 -i4
find . -name '*.h'| xargs indent -kr -brf -bli0 -i4
#find . -name '*.c'| xargs indent -gnu -brf -bli0 -i4
#find . -name '*.h'| xargs indent -gnu -brf -bli0 -i4
find . -name '*.h~' | xargs rm
find . -name '*.c~' | xargs rm
