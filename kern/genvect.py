#!/usr/bin/python
#0~47
def printFromat(trapHandle,i):
        print trapHandle + str(i) + " , " + str(i) + ")"        
for i in range(256):
    #Vect: 8/10/11/12/13/14 push error number.
    if i in [8,10,11,12,13,14]:
        printFromat("TRAPHANDLER(vect",i)
    else:
        printFromat("TRAPHANDLER_NOEC(vect",i)

for i in range(256):
    print "  .long " + "vect" + str(i);
