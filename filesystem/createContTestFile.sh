#!/bin/sh
mkdir ContTestFile
a=0;
size=1024*1024*4
while [ $a -lt 20 ]
do
	dd if=/dev/urandom of=./ContTestFile/$a".tmp" bs=$size count=1
	a=`expr $a + 1`
    echo $size
done
dd if=/dev/urandom of=./ContTestFile/"parent.tmp" bs=$size count=1



